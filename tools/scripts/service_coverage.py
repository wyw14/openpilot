#!/usr/bin/env python3
"""Summarize offline service coverage of local openpilot logs.

Prints, per service: message count, first/last logMonoTime, the largest
adjacent interval, and the nominal frequency declared in cereal/services.py.

Intervals are purely observational: no vehicle state or message-loss cause is
inferred from them. Output is sorted by service name and never contains
message payloads. Services without a declared periodic frequency, out of
order timestamps, empty/corrupt logs are explicitly marked.

Supports raw logs as well as .zst and .bz2 compressed logs.
"""

import argparse
import json
import sys

from openpilot.tools.lib.service_coverage import coverage_to_dict, format_coverage, summarize_logs


def main(argv: list[str] | None = None) -> int:
  parser = argparse.ArgumentParser(description="Offline per-service coverage summary of local openpilot logs")
  parser.add_argument("logs", nargs="+", help="path(s) to local rlog/qlog files (raw, .zst or .bz2)")
  parser.add_argument("--json", action="store_true", help="emit machine-readable JSON instead of a text table")
  args = parser.parse_args(argv)

  coverages = summarize_logs(args.logs)

  if args.json:
    print(json.dumps([coverage_to_dict(c) for c in coverages], indent=2))
  else:
    print(format_coverage(coverages))

  return 1 if any(coverage.read_error is not None for coverage in coverages) else 0


if __name__ == "__main__":
  sys.exit(main())
