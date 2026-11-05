import unittest

from openpilot.selfdrive.ui.onroad.model_health import (
  MODEL_AGE_WARN_S,
  ConfidenceLevel,
  HealthStatus,
  MONITORED_SERVICES,
  Reason,
  evaluate_model_health,
)

SERVICE_NAMES = [name for name, _, _ in MONITORED_SERVICES]
NOW = 1000.0


class FakeModel:
  def __init__(self, confidence=ConfidenceLevel.GREEN, frame_age=0, frame_drop_perc=0.0,
               model_execution_time=0.02):
    self.confidence = confidence
    self.frameAge = frame_age
    self.frameDropPerc = frame_drop_perc
    self.modelExecutionTime = model_execution_time


class FakeSM:
  """Minimal SubMaster stand-in: dict-based state plus a modelV2 payload."""

  def __init__(self, model=None):
    self.seen = dict.fromkeys(SERVICE_NAMES, False)
    self.alive = dict.fromkeys(SERVICE_NAMES, False)
    self.freq_ok = dict.fromkeys(SERVICE_NAMES, False)
    self.valid = dict.fromkeys(SERVICE_NAMES, False)
    self.recv_time = dict.fromkeys(SERVICE_NAMES, 0.0)
    self._model = model if model is not None else FakeModel()

  def mark(self, name, *, age=0.01, alive=True, freq_ok=True, valid=True):
    self.seen[name] = True
    self.alive[name] = alive
    self.freq_ok[name] = freq_ok
    self.valid[name] = valid
    self.recv_time[name] = NOW - age

  def mark_all_healthy(self):
    fresh_ages = {"modelV2": 0.02, "radarState": 0.02, "carState": 0.005,
                  "controlsState": 0.005, "extrinsicsCalibration": 0.1}
    for name in SERVICE_NAMES:
      self.mark(name, age=fresh_ages[name])

  def __getitem__(self, name):
    if name == "modelV2":
      return self._model
    raise KeyError(name)


def reason_keys(health):
  return [(r.source, r.reason) for r in health.reasons]


class TestModelHealth(unittest.TestCase):
  def test_waiting_before_anything_seen(self):
    health = evaluate_model_health(FakeSM(), now=NOW)

    self.assertFalse(health.seen)
    self.assertIsNone(health.confidence)
    self.assertEqual(health.status, HealthStatus.WAITING)
    self.assertTrue(health.ok)
    self.assertEqual(health.reasons, ())
    self.assertTrue(all(s.status == HealthStatus.WAITING for s in health.services))
    self.assertTrue(all(s.age_s < 0 for s in health.services))

  def test_green_confidence_with_fresh_inputs_is_good(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()

    health = evaluate_model_health(sm, now=NOW)

    self.assertTrue(health.seen)
    self.assertEqual(health.confidence, ConfidenceLevel.GREEN)
    self.assertEqual(health.status, HealthStatus.GOOD)
    self.assertTrue(health.ok)
    self.assertEqual(health.reasons, ())
    self.assertTrue(all(s.status == HealthStatus.GOOD for s in health.services))

  def test_yellow_confidence_warns_with_reason(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.YELLOW))
    sm.mark_all_healthy()

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertFalse(health.ok)
    self.assertIn((None, "model confidence low"), reason_keys(health))

  def test_red_confidence_is_critical_with_reason(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.RED))
    sm.mark_all_healthy()

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.CRITICAL)
    self.assertIn((None, "model confidence very low"), reason_keys(health))

  def test_dead_model_service_is_critical_and_stale(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.alive["modelV2"] = False
    sm.recv_time["modelV2"] = NOW - 5.0

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.CRITICAL)
    self.assertIn((None, "model output stale"), reason_keys(health))
    self.assertGreaterEqual(health.model_age_s, 5.0)

  def test_invalid_model_message_is_explained(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.valid["modelV2"] = False

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertIn((None, "model output invalid"), reason_keys(health))

  def test_delayed_model_message_is_explained(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.mark("modelV2", age=MODEL_AGE_WARN_S + 0.1)

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertIn((None, "model output delayed"), reason_keys(health))

  def test_pipeline_telemetry_thresholds(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN, frame_age=3,
                          frame_drop_perc=12.0, model_execution_time=0.06))
    sm.mark_all_healthy()

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.frame_age, 3)
    self.assertAlmostEqual(health.frame_drop_perc, 12.0)
    self.assertAlmostEqual(health.model_exec_time_s, 0.06)
    self.assertEqual(health.status, HealthStatus.WARNING)
    reasons = reason_keys(health)
    self.assertIn((None, "model processing old camera frames"), reasons)
    self.assertIn((None, "high camera frame drop rate"), reasons)
    self.assertIn((None, "model execution slow"), reasons)

  def test_other_service_not_receiving_is_critical_and_attributed(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.alive["radarState"] = False
    sm.recv_time["radarState"] = NOW - 3.0

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.CRITICAL)
    self.assertIn(("radar", "no data"), reason_keys(health))
    radar = next(s for s in health.services if s.name == "radarState")
    self.assertEqual(radar.status, HealthStatus.CRITICAL)

  def test_delayed_input_service_warns(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    # alive but messages arrive much slower than nominal
    sm.mark("carState", age=0.2, alive=True, freq_ok=True)

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertIn(("vehicle", "delayed"), reason_keys(health))

  def test_invalid_input_service_warns(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.valid["controlsState"] = False

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertIn(("controls", "invalid data"), reason_keys(health))

  def test_irregular_rate_warns(self):
    sm = FakeSM(FakeModel(confidence=ConfidenceLevel.GREEN))
    sm.mark_all_healthy()
    sm.freq_ok["extrinsicsCalibration"] = False

    health = evaluate_model_health(sm, now=NOW)

    self.assertEqual(health.status, HealthStatus.WARNING)
    self.assertIn(("calibration", "irregular rate"), reason_keys(health))

  def test_unknown_confidence_value_is_flagged(self):
    sm = FakeSM(FakeModel(confidence=99))
    sm.mark_all_healthy()

    health = evaluate_model_health(sm, now=NOW)

    self.assertIsNone(health.confidence)
    self.assertIn((None, "model confidence unknown"), reason_keys(health))

  def test_result_is_read_only_snapshot(self):
    # the evaluator must return immutable snapshots, nothing is written back
    health = evaluate_model_health(FakeSM(), now=NOW)
    with self.assertRaises(AttributeError):
      health.status = HealthStatus.GOOD
    self.assertIsInstance(health.reasons, tuple)
    self.assertIsInstance(health.services, tuple)
    self.assertTrue(all(isinstance(r, Reason) for r in health.reasons))


if __name__ == "__main__":
  unittest.main()
