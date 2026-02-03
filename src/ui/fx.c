#include "ui/fx.h"

static void on_event(void* user, const GameEvent* ev) {
  FxSystem* fx = (FxSystem*)user;
  if (!fx || !ev) return;

  if (ev->type == EV_LINE_CLEAR) {
    const int cleared = ev->a;
    if (cleared == 4) {
      fx->flash_ms = 120;
      fx->flash_on = 1;
    }
  }
}

void fx_init(FxSystem* fx, EventBus* bus) {
  if (!fx) return;
  fx->flash_ms = 0;
  fx->flash_on = 0;
  if (bus) (void)event_bus_subscribe(bus, fx, on_event);
}

void fx_update(FxSystem* fx, int dt_ms) {
  if (!fx) return;
  if (fx->flash_ms > 0) {
    fx->flash_ms -= dt_ms;
    if (fx->flash_ms <= 0) { fx->flash_ms = 0; fx->flash_on = 0; }
  }
}

void fx_render_overlay(const FxSystem* fx, Renderer* rd) {
  if (!fx || !rd) return;
  if (fx->flash_on) {
    draw_rect(rd, 0, 0, rd->logical_w, rd->logical_h, (ColorRGBA){255,255,255,35}, true);
  }
}