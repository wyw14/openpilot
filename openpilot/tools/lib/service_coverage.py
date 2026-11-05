#!/usr/bin/env python3
"""Offline service coverage summary for local openpilot logs.

For every service (event union type) observed in a log, this reports:
  - message count
  - first and last logMonoTime (in file/stream order)
  - maximum interval between adjacent messages of that service
  - the nominal frequency declared in openpilot/cereal/services.py

All intervals are observational measurements of the recorded timestamps only.
They are not compared against the nominal frequency to estimate dropped
messages, and nothing here infers vehicle state or the cause of any gap.

Message payloads are never read or included in the output.

File-level markers flag empty logs, logs whose tail could not be decoded
(corrupt tail), individual events that failed to decode, services without a
declared periodic frequency, and services whose timestamps are not
monotonic in file/stream order (out of order).
"""

from __future__ import annotations

import os
import warnings

import capnp
import zstandard as zstd

from collections import OrderedDict
from dataclasses import dataclass
from typing import Any

from openpilot.cereal.services import SERVICE_LIST
from openpilot.tools.lib.logreader import LogReader, LogsUnavailable

# logs whose decoding aborts before all bytes are consumed emit this warning
CORRUPT_TAIL_WARNING = "Corrupted events detected"


@dataclass(frozen=True)
class ServiceCoverage:
  service: str
  count: int
  first_log_mono_time: int | None
  last_log_mono_time: int | None
  # largest delta between adjacent messages in file/stream order (nanoseconds);
  # can be negative when timestamps are out of order
  max_interval_ns: int | None
  out_of_order: bool
  # nominal frequency declared in services.py; None when undeclared/aperiodic
  nominal_frequency_hz: float | None

  @property
  def has_declared_frequency(self) -> bool:
    return self.nominal_frequency_hz is not None

  def flags(self) -> tuple[str, ...]:
    flags = []
    if self.out_of_order:
      flags.append("out_of_order")
    if not self.has_declared_frequency:
      flags.append("no_declared_frequency")
    return tuple(flags)


@dataclass(frozen=True)
class LogCoverage:
  path: str
  event_count: int
  services: list[ServiceCoverage]
  # parsing stopped before the end of the file (undecodable/corrupt tail bytes)
  truncated: bool
  # number of events that could not be decoded/attributed to a service
  corrupt_events: int
  # error string when the file could not be opened/decompressed at all
  read_error: str | None

  @property
  def empty(self) -> bool:
    return self.read_error is None and self.event_count == 0


class _ServiceAccumulator:
  __slots__ = ("count", "first_log_mono_time", "last_log_mono_time", "prev_log_mono_time", "max_interval_ns", "out_of_order")

  def __init__(self):
    self.count = 0
    self.first_log_mono_time: int | None = None
    self.last_log_mono_time: int | None = None
    self.prev_log_mono_time: int | None = None
    self.max_interval_ns: int | None = None
    self.out_of_order = False

  def add(self, log_mono_time: int) -> None:
    if self.count == 0:
      self.first_log_mono_time = log_mono_time
    else:
      interval_ns = log_mono_time - self.prev_log_mono_time
      if interval_ns < 0:
        self.out_of_order = True
      if self.max_interval_ns is None or interval_ns > self.max_interval_ns:
        self.max_interval_ns = interval_ns
    self.prev_log_mono_time = log_mono_time
    self.last_log_mono_time = log_mono_time
    self.count += 1


def _nominal_frequency(service: str) -> float | None:
  declared = SERVICE_LIST.get(service)
  if declared is None or declared.frequency <= 0.0:
    return None
  return declared.frequency


def summarize_log(path: str) -> LogCoverage:
  """Summarize message coverage per service for one local log file.

  Supports raw logs as well as .zst/.bz2 compressed logs (via LogReader).
  """
  services: OrderedDict[str, _ServiceAccumulator] = OrderedDict()
  event_count = 0
  corrupt_events = 0
  truncated = False
  read_error: str | None = None

  if not os.path.isfile(path):
    return LogCoverage(
      path=path,
      event_count=0,
      services=[],
      truncated=False,
      corrupt_events=0,
      read_error=f"FileNotFoundError: not a local file: {path}",
    )

  try:
    reader = LogReader(path)
    with warnings.catch_warnings(record=True) as caught:
      warnings.simplefilter("always", RuntimeWarning)
      for msg in reader:
        try:
          service = msg.which()
          log_mono_time = int(msg.logMonoTime)
        except capnp.KjException:
          corrupt_events += 1
          continue
        services.setdefault(service, _ServiceAccumulator()).add(log_mono_time)
        event_count += 1
    truncated = any(issubclass(w.category, RuntimeWarning) and CORRUPT_TAIL_WARNING in str(w.message) for w in caught)
  except (OSError, ValueError, EOFError, capnp.KjException, zstd.ZstdError, LogsUnavailable) as e:
    read_error = f"{type(e).__name__}: {e}"

  return LogCoverage(
    path=path,
    event_count=event_count,
    services=_sorted_service_coverage(services),
    truncated=truncated,
    corrupt_events=corrupt_events,
    read_error=read_error,
  )


def summarize_logs(paths: list[str]) -> list[LogCoverage]:
  return [summarize_log(path) for path in paths]


def _sorted_service_coverage(services: OrderedDict[str, _ServiceAccumulator]) -> list[ServiceCoverage]:
  # stable sort by service name (keys are unique, so this is deterministic)
  return [
    ServiceCoverage(
      service=name,
      count=acc.count,
      first_log_mono_time=acc.first_log_mono_time,
      last_log_mono_time=acc.last_log_mono_time,
      max_interval_ns=acc.max_interval_ns,
      out_of_order=acc.out_of_order,
      nominal_frequency_hz=_nominal_frequency(name),
    )
    for name, acc in sorted(services.items(), key=lambda kv: kv[0])
  ]


def _format_ns_interval(interval_ns: int | None) -> str:
  if interval_ns is None:
    return "-"
  return f"{interval_ns / 1e9:.6f}"


def _format_frequency(frequency_hz: float | None) -> str:
  if frequency_hz is None:
    return "-"
  return f"{frequency_hz:g}"


def format_coverage(coverages: list[LogCoverage]) -> str:
  """Render coverage summaries as a human-readable, payload-free text report."""
  blocks = []
  header_cols = ("service", "count", "first_logMonoTime", "last_logMonoTime", "max_interval_s", "nominal_hz", "flags")

  for coverage in coverages:
    lines = [coverage.path]

    file_flags = []
    if coverage.read_error is not None:
      file_flags.append(f"read_error: {coverage.read_error}")
    if coverage.empty:
      file_flags.append("empty")
    if coverage.truncated:
      file_flags.append("corrupt_tail")
    if coverage.corrupt_events:
      file_flags.append(f"corrupt_events: {coverage.corrupt_events}")
    out_of_order = [s.service for s in coverage.services if s.out_of_order]
    if out_of_order:
      file_flags.append("out_of_order: " + ",".join(out_of_order))

    lines.append(f"  events: {coverage.event_count}" + (f"  ({'; '.join(file_flags)})" if file_flags else ""))

    rows = [header_cols]
    for service in coverage.services:
      rows.append(
        (
          service.service,
          str(service.count),
          str(service.first_log_mono_time),
          str(service.last_log_mono_time),
          _format_ns_interval(service.max_interval_ns),
          _format_frequency(service.nominal_frequency_hz),
          ",".join(service.flags()),
        )
      )

    widths = [max(len(row[col]) for row in rows) for col in range(len(header_cols))]
    for row_index, row in enumerate(rows):
      lines.append("  " + "  ".join(value.ljust(widths[col]) for col, value in enumerate(row)).rstrip())
      if row_index == 0:
        lines.append("  " + "  ".join("-" * widths[col] for col in range(len(header_cols))))

    blocks.append("\n".join(lines))

  return "\n\n".join(blocks)


def coverage_to_dict(coverage: LogCoverage) -> dict[str, Any]:
  return {
    "path": coverage.path,
    "event_count": coverage.event_count,
    "empty": coverage.empty,
    "truncated": coverage.truncated,
    "corrupt_events": coverage.corrupt_events,
    "read_error": coverage.read_error,
    "services": [
      {
        "service": service.service,
        "count": service.count,
        "first_log_mono_time": service.first_log_mono_time,
        "last_log_mono_time": service.last_log_mono_time,
        "max_interval_ns": service.max_interval_ns,
        "out_of_order": service.out_of_order,
        "nominal_frequency_hz": service.nominal_frequency_hz,
      }
      for service in coverage.services
    ],
  }
