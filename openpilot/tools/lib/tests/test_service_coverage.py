import bz2
import json
import os
import tempfile
import unittest
import zstandard as zstd

from openpilot.cereal import log as capnp_log
from openpilot.tools.lib import service_coverage as sc
from openpilot.tools.lib.service_coverage import format_coverage, summarize_log, summarize_logs


# union members that are lists need an explicit element count for init()
LIST_SERVICES = {"can", "sendcan", "pandaStates", "onroadEvents"}


def make_event(service: str, log_mono_time: int) -> bytes:
  event = capnp_log.Event.new_message()
  event.logMonoTime = log_mono_time
  if service in LIST_SERVICES:
    event.init(service, 0)
  else:
    event.init(service)
  return event.to_bytes()


def corrupt_union_event() -> bytes:
  # an Event whose union discriminant is out of range: iterable, but which() raises
  event = capnp_log.Event.new_message()
  raw = bytearray(event.to_bytes())
  raw[event.total_size.word_count * 8] = 0xFF
  return bytes(raw)


class TestServiceCoverage(unittest.TestCase):
  def setUp(self):
    super().setUp()
    self.tmpdir = tempfile.TemporaryDirectory()
    self.addCleanup(self.tmpdir.cleanup)

  def write_log(self, name: str, events: list[bytes], compress: str | None = None) -> str:
    path = os.path.join(self.tmpdir.name, name)
    data = b"".join(events)
    if compress == ".zst":
      data = zstd.compress(data)
    elif compress == ".bz2":
      data = bz2.compress(data)
    with open(path, "wb") as f:
      f.write(data)
    return path

  def test_counts_first_last_and_max_interval(self):
    events = [
      make_event("controlsState", 1_000),
      make_event("controlsState", 1_010),
      make_event("controlsState", 1_030),
      make_event("controlsState", 1_060),
      make_event("userBookmark", 2_000),  # declared aperiodic (frequency 0)
    ]
    path = self.write_log("rlog", events)

    coverage = summarize_log(path)
    by_name = {s.service: s for s in coverage.services}

    self.assertEqual(coverage.event_count, 5)
    self.assertFalse(coverage.truncated)
    self.assertEqual(coverage.corrupt_events, 0)
    self.assertIsNone(coverage.read_error)
    self.assertFalse(coverage.empty)

    controls = by_name["controlsState"]
    self.assertEqual(controls.count, 4)
    self.assertEqual(controls.first_log_mono_time, 1_000)
    self.assertEqual(controls.last_log_mono_time, 1_060)
    self.assertEqual(controls.max_interval_ns, 30)  # gaps: 10, 20, 30
    self.assertFalse(controls.out_of_order)
    self.assertEqual(controls.nominal_frequency_hz, 100.0)

    user_bookmark = by_name["userBookmark"]
    self.assertEqual(user_bookmark.count, 1)
    self.assertEqual(user_bookmark.first_log_mono_time, 2_000)
    self.assertEqual(user_bookmark.last_log_mono_time, 2_000)
    self.assertIsNone(user_bookmark.max_interval_ns)
    self.assertIsNone(user_bookmark.nominal_frequency_hz)
    self.assertIn("no_declared_frequency", user_bookmark.flags())

  def test_out_of_order_timestamps_are_marked(self):
    # intervals are taken in file/stream order: gaps are +200 and -100
    events = [
      make_event("deviceState", 100),
      make_event("can", 50),
      make_event("deviceState", 300),
      make_event("deviceState", 200),
    ]
    path = self.write_log("rlog", events)

    coverage = summarize_log(path)
    by_name = {s.service: s for s in coverage.services}

    device_state = by_name["deviceState"]
    self.assertTrue(device_state.out_of_order)
    self.assertEqual(device_state.first_log_mono_time, 100)
    self.assertEqual(device_state.last_log_mono_time, 200)
    self.assertEqual(device_state.max_interval_ns, 200)
    self.assertIn("out_of_order", device_state.flags())

    can = by_name["can"]
    self.assertFalse(can.out_of_order)
    self.assertIsNone(can.max_interval_ns)

  def test_empty_log(self):
    path = self.write_log("rlog", [])

    coverage = summarize_log(path)

    self.assertTrue(coverage.empty)
    self.assertEqual(coverage.event_count, 0)
    self.assertEqual(coverage.services, [])
    self.assertFalse(coverage.truncated)
    self.assertIsNone(coverage.read_error)
    self.assertIn("empty", format_coverage([coverage]))

  def test_compressed_logs_match_raw(self):
    events = [make_event("controlsState", 1_000 + i * 10_000_000) for i in range(10)]
    paths = [
      self.write_log("rlog", events),
      self.write_log("qlog.zst", events, compress=".zst"),
      self.write_log("qlog.bz2", events, compress=".bz2"),
    ]

    coverages = summarize_logs(paths)
    for coverage in coverages:
      self.assertIsNone(coverage.read_error, msg=coverage.path)
      self.assertFalse(coverage.empty)
      self.assertEqual(coverage.event_count, 10)
      (service,) = coverage.services
      self.assertEqual(service.service, "controlsState")
      self.assertEqual(service.count, 10)
      self.assertEqual(service.first_log_mono_time, 1_000)
      self.assertEqual(service.last_log_mono_time, 1_000 + 9 * 10_000_000)
      self.assertEqual(service.max_interval_ns, 10_000_000)
      self.assertEqual(service.nominal_frequency_hz, 100.0)

  def test_corrupt_tail_is_marked_and_valid_events_kept(self):
    events = [make_event("controlsState", 1_000 + i * 10_000_000) for i in range(3)]
    # compress a stream that itself ends with garbage bytes
    path = self.write_log("rlog.zst", events + [b"\xff\xff\xff garbage tail \x00\x00"], compress=".zst")

    coverage = summarize_log(path)

    self.assertTrue(coverage.truncated)
    self.assertIsNone(coverage.read_error)
    self.assertIn("corrupt_tail", format_coverage([coverage]))
    (service,) = coverage.services
    self.assertEqual(service.service, "controlsState")
    self.assertEqual(service.count, 3)

  def test_corrupt_event_is_counted_but_not_attributed(self):
    events = [
      make_event("controlsState", 1_000),
      corrupt_union_event(),
      make_event("controlsState", 1_010),
    ]
    path = self.write_log("rlog", events)

    coverage = summarize_log(path)

    self.assertEqual(coverage.event_count, 2)
    self.assertEqual(coverage.corrupt_events, 1)
    (service,) = coverage.services
    self.assertEqual(service.service, "controlsState")
    self.assertEqual(service.count, 2)
    self.assertEqual(service.max_interval_ns, 10)

  def test_undeclared_services_are_marked(self):
    # sentinel exists in the Event schema but is not declared in services.py
    events = [make_event("sentinel", 1), make_event("sentinel", 2)]
    path = self.write_log("rlog", events)

    coverage = summarize_log(path)
    (service,) = coverage.services
    self.assertEqual(service.service, "sentinel")
    self.assertIsNone(service.nominal_frequency_hz)
    self.assertIn("no_declared_frequency", service.flags())

  def test_services_are_stably_sorted_by_name(self):
    events = [
      make_event("selfdriveState", 1),
      make_event("can", 1),
      make_event("controlsState", 1),
      make_event("accelerometer", 1),
    ]
    path = self.write_log("rlog", events)
    # run twice: ordering must not depend on insertion order or dict hashing
    names_first = [s.service for s in summarize_log(path).services]
    names_second = [s.service for s in summarize_log(path).services]

    self.assertEqual(names_first, ["accelerometer", "can", "controlsState", "selfdriveState"])
    self.assertEqual(names_first, names_second)

  def test_missing_file_is_reported(self):
    coverage = summarize_log(os.path.join(self.tmpdir.name, "does_not_exist"))
    self.assertIsNotNone(coverage.read_error)
    self.assertEqual(coverage.event_count, 0)
    self.assertEqual(coverage.services, [])

  def test_output_contains_no_payload_and_is_json_serializable(self):
    events = [
      make_event("carParams", 1_000),
      make_event("carParams", 1_020),
      make_event("userBookmark", 2_000),
    ]
    path = self.write_log("rlog", events)

    as_dict = sc.coverage_to_dict(summarize_log(path))
    self.assertEqual(set(as_dict.keys()), {"path", "event_count", "empty", "truncated", "corrupt_events", "read_error", "services"})
    for service in as_dict["services"]:
      self.assertEqual(
        set(service.keys()), {"service", "count", "first_log_mono_time", "last_log_mono_time", "max_interval_ns", "out_of_order", "nominal_frequency_hz"}
      )
    # round-trips without any payload-bearing types
    json.loads(json.dumps(as_dict))

    text = format_coverage([summarize_log(path)])
    self.assertIn("carParams", text)
    self.assertIn("logMonoTime", text)
    self.assertIn("nominal_hz", text)


if __name__ == "__main__":
  unittest.main()
