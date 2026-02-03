#pragma once
#include <stdbool.h>
#include "core/state.h"
#include "core/event_bus.h"
#include "core/time.h"
#include "render/renderer.h"
#include "input/input.h"
#include "utils/config.h"

typedef struct App {
  Renderer renderer;
  Input input;
  EventBus bus;
  Config cfg;
  TimeStep time;
  StateStack states;
  bool running;
  char config_path[64];
} App;

bool app_init(App* app);
void app_shutdown(App* app);
void app_run(App* app);

// helpers
void app_quit(App* app);
void app_save_config(App* app);