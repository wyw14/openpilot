import raylib_nb as rl

from openpilot.selfdrive.ui.onroad.model_health import (
  ConfidenceLevel,
  HealthStatus,
  ModelHealth,
  evaluate_model_health,
)
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.system.ui.lib.application import gui_app, FontWeight
from openpilot.system.ui.lib.multilang import tr
from openpilot.system.ui.lib.text_measure import measure_text_cached
from openpilot.system.ui.lib.wrap_text import wrap_text
from openpilot.system.ui.widgets import Widget

# Advisory diagnostic overlay for the driving model. It only reads messages the
# UI already subscribes to and never writes params or publishes messages, so it
# has no effect on control output.

PANEL_WIDTH = 430
PANEL_MARGIN = 40
PADDING = 24
TITLE_SIZE = 28
TEXT_SIZE = 26
TELEMETRY_SIZE = 22
REASON_SIZE = 24
ROW_HEIGHT = 36
DOT_RADIUS = 7

BG_COLOR = rl.Color(0, 0, 0, 190)
BORDER_COLOR = rl.Color(255, 255, 255, 45)
TEXT_COLOR = rl.Color(230, 230, 230, 255)
DIM_COLOR = rl.Color(150, 150, 150, 255)

STATUS_COLORS = {
  HealthStatus.WAITING: rl.Color(145, 155, 149, 255),
  HealthStatus.GOOD: rl.Color(128, 216, 166, 255),
  HealthStatus.WARNING: rl.Color(255, 193, 7, 255),
  HealthStatus.CRITICAL: rl.Color(225, 65, 65, 255),
}

# English keys produced by model_health. Every value is a literal tr() call so
# the translation extractor (selfdrive/ui/translations/update_translations.py)
# can discover the strings.
STATUS_KEY = {
  HealthStatus.WAITING: "waiting",
  HealthStatus.GOOD: "normal",
  HealthStatus.WARNING: "degraded",
  HealthStatus.CRITICAL: "critical",
}

CONFIDENCE_KEY = {
  ConfidenceLevel.GREEN: "high",
  ConfidenceLevel.YELLOW: "reduced",
  ConfidenceLevel.RED: "low",
}

CONFIDENCE_STATUS = {
  ConfidenceLevel.GREEN: HealthStatus.GOOD,
  ConfidenceLevel.YELLOW: HealthStatus.WARNING,
  ConfidenceLevel.RED: HealthStatus.CRITICAL,
}


def _tr(key: str | None) -> str:
  if key is None:
    return ""
  return {
    # overall status
    "waiting": tr("waiting"),
    "normal": tr("normal"),
    "degraded": tr("degraded"),
    "critical": tr("critical"),
    # confidence
    "high": tr("high"),
    "reduced": tr("reduced"),
    "low": tr("low"),
    # service labels
    "driving model": tr("driving model"),
    "radar": tr("radar"),
    "vehicle": tr("vehicle"),
    "controls": tr("controls"),
    "calibration": tr("calibration"),
    # model-level reasons
    "model output stale": tr("model output stale"),
    "model output invalid": tr("model output invalid"),
    "model output delayed": tr("model output delayed"),
    "model confidence unknown": tr("model confidence unknown"),
    "model confidence very low": tr("model confidence very low"),
    "model confidence low": tr("model confidence low"),
    "model processing old camera frames": tr("model processing old camera frames"),
    "high camera frame drop rate": tr("high camera frame drop rate"),
    "model execution slow": tr("model execution slow"),
    # per-service reasons
    "no data": tr("no data"),
    "invalid data": tr("invalid data"),
    "delayed": tr("delayed"),
    "irregular rate": tr("irregular rate"),
  }.get(key, key)


class ModelHealthOverlay(Widget):
  def __init__(self):
    super().__init__()
    self._font_bold = gui_app.font(FontWeight.BOLD)
    self._font_medium = gui_app.font(FontWeight.MEDIUM)
    self._font_normal = gui_app.font(FontWeight.NORMAL)
    self._health: ModelHealth | None = None

  def _update_state(self) -> None:
    if ui_state.started and ui_state.show_model_confidence:
      self._health = evaluate_model_health(ui_state.sm)
    else:
      self._health = None

  def _render(self, rect: rl.Rectangle) -> None:
    health = self._health
    if health is None:
      return

    x = rect.x + rect.width - PANEL_WIDTH - PANEL_MARGIN
    w = PANEL_WIDTH
    inner_w = w - 2 * PADDING
    y_cursor = rect.y + PANEL_MARGIN + PADDING

    # Measure everything first so the panel wraps its content.
    title = tr("Model Health")
    status_text = _tr(STATUS_KEY[health.status])
    status_size = measure_text_cached(self._font_bold, status_text, TITLE_SIZE)

    reason_lines: list[str] = []
    for reason in health.reasons:
      text = _tr(reason.reason) if reason.source is None else f"{_tr(reason.source)}: {_tr(reason.reason)}"
      reason_lines.extend(wrap_text(self._font_normal, f"• {text}", REASON_SIZE, inner_w))

    telemetry = (f"{tr('frame age')} {health.frame_age}    {tr('drops')} "
                 f"{health.frame_drop_perc:.0f}%    exec {health.model_exec_time_s * 1000:.0f} ms")

    row_count = 1 + len(health.services)
    height = (PADDING + TITLE_SIZE + 14 +
              row_count * ROW_HEIGHT + 12 +
              TELEMETRY_SIZE + 16 +
              len(reason_lines) * (REASON_SIZE + 4) + PADDING)

    panel_y = rect.y + PANEL_MARGIN
    panel = rl.Rectangle(x, panel_y, w, height)
    rl.draw_rectangle_rounded(panel, 0.06, 12, BG_COLOR)
    rl.draw_rectangle_rounded_lines_ex(panel, 0.06, 12, 2, BORDER_COLOR)

    # Title + overall status
    rl.draw_text_ex(self._font_bold, title, rl.Vector2(x + PADDING, y_cursor), TITLE_SIZE, 0, TEXT_COLOR)
    rl.draw_text_ex(
      self._font_bold, status_text,
      rl.Vector2(x + w - PADDING - status_size.x, y_cursor),
      TITLE_SIZE, 0, STATUS_COLORS[health.status],
    )
    y_cursor += TITLE_SIZE + 14

    # Confidence row
    if health.confidence is None:
      value_text, value_status = _tr("waiting"), HealthStatus.WAITING
    else:
      value_text = _tr(CONFIDENCE_KEY[health.confidence])
      value_status = CONFIDENCE_STATUS[health.confidence]
    value_size = measure_text_cached(self._font_medium, value_text, TEXT_SIZE)
    self._draw_dot(x + PADDING + DOT_RADIUS, y_cursor + TEXT_SIZE / 2, STATUS_COLORS[value_status])
    rl.draw_text_ex(self._font_medium, tr("confidence"),
                    rl.Vector2(x + PADDING + 2 * DOT_RADIUS + 12, y_cursor), TEXT_SIZE, 0, TEXT_COLOR)
    rl.draw_text_ex(
      self._font_medium, value_text,
      rl.Vector2(x + w - PADDING - value_size.x, y_cursor),
      TEXT_SIZE, 0, STATUS_COLORS[value_status],
    )
    y_cursor += ROW_HEIGHT

    # Per-service freshness rows
    for service in health.services:
      age_text = "—" if service.age_s < 0 else f"{service.age_s * 1000:.0f} ms"
      age_size = measure_text_cached(self._font_medium, age_text, TEXT_SIZE)
      self._draw_dot(x + PADDING + DOT_RADIUS, y_cursor + TEXT_SIZE / 2, STATUS_COLORS[service.status])
      rl.draw_text_ex(self._font_medium, _tr(service.label),
                      rl.Vector2(x + PADDING + 2 * DOT_RADIUS + 12, y_cursor), TEXT_SIZE, 0, TEXT_COLOR)
      rl.draw_text_ex(
        self._font_medium, age_text,
        rl.Vector2(x + w - PADDING - age_size.x, y_cursor),
        TEXT_SIZE, 0, STATUS_COLORS[service.status],
      )
      y_cursor += ROW_HEIGHT

    # Pipeline telemetry
    y_cursor += 12
    rl.draw_text_ex(self._font_normal, telemetry,
                    rl.Vector2(x + PADDING, y_cursor), TELEMETRY_SIZE, 0, DIM_COLOR)
    y_cursor += TELEMETRY_SIZE + 16

    # Explanations for anything below threshold
    if reason_lines:
      for line in reason_lines:
        rl.draw_text_ex(self._font_normal, line,
                        rl.Vector2(x + PADDING, y_cursor), REASON_SIZE, 0, STATUS_COLORS[HealthStatus.WARNING])
        y_cursor += REASON_SIZE + 4

  @staticmethod
  def _draw_dot(center_x: float, center_y: float, color: rl.Color) -> None:
    rl.draw_circle(int(center_x), int(center_y), DOT_RADIUS, color)
