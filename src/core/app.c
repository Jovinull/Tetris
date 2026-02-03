#include "core/app.h"
#include "utils/log.h"
#include "utils/str.h"
#include "ui/state_menu.h"
#include <string.h>

bool app_init(App* app) {
  if (!app) return false;
  memset(app, 0, sizeof(*app));

  (void)str_snprintf(app->config_path, sizeof(app->config_path), "config.ini");
  config_set_defaults(&app->cfg);
  (void)config_load_ini(&app->cfg, app->config_path);
  (void)config_save_ini(&app->cfg, app->config_path); // garante arquivo

#ifndef TETRIS_NO_DEBUG
  log_set_level(LOG_DEBUG);
#else
  log_set_level(LOG_INFO);
#endif

  event_bus_init(&app->bus);
  state_stack_init(&app->states);

  // janela lógica: espaço p/ HUD lateral
  const int logical_w = 420;
  const int logical_h = 300;

  if (!renderer_init(&app->renderer, "Tetris C11", logical_w, logical_h, app->cfg.fullscreen, app->cfg.scale)) {
    return false;
  }

  input_init(&app->input, &app->cfg);

  time_init(&app->time, 1.0 / 60.0);

  app->running = true;

  // push menu inicial
  if (!state_stack_push(&app->states, app, menu_state_create())) {
    return false;
  }
  return true;
}

void app_shutdown(App* app) {
  if (!app) return;
  while (state_stack_top(&app->states)) {
    (void)state_stack_pop(&app->states, app);
  }
  renderer_shutdown(&app->renderer);
}

void app_quit(App* app) {
  if (!app) return;
  app->running = false;
}

void app_save_config(App* app) {
  if (!app) return;
  (void)config_save_ini(&app->cfg, app->config_path);
}

void app_run(App* app) {
  if (!app) return;

  SDL_Event e;

  while (app->running) {
    (void)time_begin_frame(&app->time);

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        app_quit(app);
        break;
      }
      input_handle_event(&app->input, &e);
    }

    // update input repeats at ~16ms (fixed tick)
    input_update(&app->input, 16);

    {
      State* s = state_stack_top(&app->states);
      if (s && s->v && s->v->handle_event) s->v->handle_event(s, app, NULL);
    }

    while (time_should_update(&app->time)) {
      State* s = state_stack_top(&app->states);
      if (s && s->v && s->v->update) s->v->update(s, app, app->time.dt_fixed);
      time_consume_tick(&app->time);
    }

    renderer_begin(&app->renderer);
    State* s = state_stack_top(&app->states);
    if (s && s->v && s->v->render) s->v->render(s, app);
    renderer_present(&app->renderer);
  }
}