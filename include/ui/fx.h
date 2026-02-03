#pragma once
#include "core/event_bus.h"
#include "render/renderer.h"

typedef struct FxSystem {
  int flash_ms;
  int flash_on;
} FxSystem;

void fx_init(FxSystem* fx, EventBus* bus);
void fx_update(FxSystem* fx, int dt_ms);
void fx_render_overlay(const FxSystem* fx, Renderer* rd);