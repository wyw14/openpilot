"""Advisory safety-model health for the onroad diagnostic overlay.

The functions here turn data the UI has already subscribed to (modelV2 and the
other safety-critical services) into a model confidence / data freshness view.

Everything in this module is display-only: it performs no parameter writes,
publishes no messages, and never feeds back into controlsd/plannerd, so a low
score can explain a situation on screen without changing vehicle control.

The module intentionally only depends on the standard library so it can be
unit tested without msgq, capnp or a display.
"""

import time
from dataclasses import dataclass
from enum import IntEnum
from typing import Any, Protocol

MODEL_SERVICE = "modelV2"

# (service name, translation key for display label, nominal period in seconds)
# Frequencies mirror cereal/services.py.
MONITORED_SERVICES: tuple[tuple[str, str, float], ...] = (
  ("modelV2", "driving model", 1 / 20.0),
  ("radarState", "radar", 1 / 20.0),
  ("carState", "vehicle", 1 / 100.0),
  ("controlsState", "controls", 1 / 100.0),
  ("extrinsicsCalibration", "calibration", 1 / 4.0),
)

# A message older than this many nominal periods is considered delayed (warning).
STALE_AGE_MULTIPLIER = 5.0

# modelV2 runs at 20 Hz, anything older than a few frames is worth flagging
MODEL_AGE_WARN_S = 5 / 20.0
FRAME_AGE_WARN = 2
FRAME_DROP_WARN_PERC = 5.0
MODEL_EXEC_TIME_WARN_S = 0.04  # modeld's 20 Hz frame budget is 50 ms


class ConfidenceLevel(IntEnum):
  # Mirrors ModelDataV2.ConfidenceClass in cereal/log.capnp
  RED = 0
  YELLOW = 1
  GREEN = 2


class HealthStatus(IntEnum):
  WAITING = 0
  GOOD = 1
  WARNING = 2
  CRITICAL = 3


@dataclass(frozen=True)
class Reason:
  """A single explanation for a non-good status.

  source is the translation key of the affected service label, or None for
  reasons about the driving model itself.
  """
  reason: str
  source: str | None = None


@dataclass(frozen=True)
class ServiceHealth:
  name: str
  label: str
  seen: bool
  alive: bool
  freq_ok: bool
  valid: bool
  age_s: float
  warn_age_s: float
  status: HealthStatus
  reason: str | None


@dataclass(frozen=True)
class ModelHealth:
  seen: bool
  confidence: ConfidenceLevel | None
  status: HealthStatus
  model_age_s: float
  frame_age: int
  frame_drop_perc: float
  model_exec_time_s: float
  services: tuple[ServiceHealth, ...]
  reasons: tuple[Reason, ...]

  @property
  def ok(self) -> bool:
    return self.status <= HealthStatus.GOOD


class SubMasterLike(Protocol):
  seen: dict[str, bool]
  alive: dict[str, bool]
  freq_ok: dict[str, bool]
  valid: dict[str, bool]
  recv_time: dict[str, float]

  def __getitem__(self, service: str) -> Any: ...


def _evaluate_service(name: str, label: str, period_s: float, sm: Any, now: float) -> ServiceHealth:
  seen = bool(sm.seen.get(name, False))
  alive = bool(sm.alive.get(name, False))
  freq_ok = bool(sm.freq_ok.get(name, False))
  valid = bool(sm.valid.get(name, False))
  recv_time = float(sm.recv_time.get(name, 0.0) or 0.0)
  age_s = (now - recv_time) if seen and recv_time > 0.0 else -1.0
  warn_age_s = max(STALE_AGE_MULTIPLIER * period_s, 0.05)

  if not seen:
    return ServiceHealth(name, label, False, False, False, False, -1.0, warn_age_s,
                         HealthStatus.WAITING, None)

  if not alive:
    status, reason = HealthStatus.CRITICAL, "no data"
  elif not valid:
    status, reason = HealthStatus.WARNING, "invalid data"
  elif age_s > warn_age_s:
    status, reason = HealthStatus.WARNING, "delayed"
  elif not freq_ok:
    status, reason = HealthStatus.WARNING, "irregular rate"
  else:
    status, reason = HealthStatus.GOOD, None

  return ServiceHealth(name, label, True, alive, freq_ok, valid, age_s, warn_age_s, status, reason)


def evaluate_model_health(sm: SubMasterLike, now: float | None = None) -> ModelHealth:
  """Evaluate driving model confidence and freshness of its inputs.

  The result is purely advisory; callers render it but must not act on it.
  """
  if now is None:
    now = time.monotonic()

  services = tuple(
    _evaluate_service(name, label, period_s, sm, now)
    for name, label, period_s in MONITORED_SERVICES
  )

  model_seen = bool(sm.seen.get(MODEL_SERVICE, False))
  model_service = next(s for s in services if s.name == MODEL_SERVICE)

  reasons: list[Reason] = []
  statuses: list[HealthStatus] = [s.status for s in services]

  confidence: ConfidenceLevel | None = None
  model_age_s = model_service.age_s
  frame_age = 0
  frame_drop_perc = 0.0
  model_exec_time_s = 0.0

  if model_seen:
    model = sm[MODEL_SERVICE]

    # model output freshness / validity
    if not model_service.alive:
      reasons.append(Reason("model output stale"))
    elif not model_service.valid:
      reasons.append(Reason("model output invalid"))
    elif model_age_s > MODEL_AGE_WARN_S:
      reasons.append(Reason("model output delayed"))

    # model self-assessment
    try:
      confidence = ConfidenceLevel(int(model.confidence))
    except (AttributeError, TypeError, ValueError):
      confidence = None
      reasons.append(Reason("model confidence unknown"))
    else:
      if confidence == ConfidenceLevel.RED:
        statuses.append(HealthStatus.CRITICAL)
        reasons.append(Reason("model confidence very low"))
      elif confidence == ConfidenceLevel.YELLOW:
        statuses.append(HealthStatus.WARNING)
        reasons.append(Reason("model confidence low"))

    # pipeline telemetry explaining *why* confidence/freshness dropped
    frame_age = int(getattr(model, "frameAge", 0) or 0)
    if frame_age >= FRAME_AGE_WARN:
      statuses.append(HealthStatus.WARNING)
      reasons.append(Reason("model processing old camera frames"))

    frame_drop_perc = float(getattr(model, "frameDropPerc", 0.0) or 0.0)
    if frame_drop_perc > FRAME_DROP_WARN_PERC:
      statuses.append(HealthStatus.WARNING)
      reasons.append(Reason("high camera frame drop rate"))

    model_exec_time_s = float(getattr(model, "modelExecutionTime", 0.0) or 0.0)
    if model_exec_time_s > MODEL_EXEC_TIME_WARN_S:
      statuses.append(HealthStatus.WARNING)
      reasons.append(Reason("model execution slow"))
  else:
    statuses.append(HealthStatus.WAITING)

  # surface problems of the other safety inputs (modelV2 already explained above)
  for service in services:
    if service.name == MODEL_SERVICE or service.reason is None:
      continue
    statuses.append(service.status)
    reasons.append(Reason(service.reason, source=service.label))

  return ModelHealth(
    seen=model_seen,
    confidence=confidence,
    status=max(statuses, default=HealthStatus.WAITING),
    model_age_s=model_age_s,
    frame_age=frame_age,
    frame_drop_perc=frame_drop_perc,
    model_exec_time_s=model_exec_time_s,
    services=services,
    reasons=tuple(reasons),
  )
