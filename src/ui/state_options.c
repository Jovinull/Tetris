#include "ui/state_options.h"
#include "core/app.h"
#include "utils/str.h"
#include <stdlib.h>
#include <string.h>

typedef struct OptionsState {
  State base;
  int sel;
} OptionsState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e;
  OptionsState* s = (OptionsState*)ss;

  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (!cmd.pressed) continue;

    if (cmd.action == ACT_DOWN) s->sel = (s->sel + 1) % 7;
    if (cmd.action == ACT_UP) s->sel = (s->sel - 1 + 7) % 7;
    if (cmd.action == ACT_BACK || cmd.action == ACT_PAUSE) { app_save_config(app); (void)state_stack_pop(&app->states, app); }

    if (cmd.action == ACT_CONFIRM) {
      if (s->sel == 0) { // mode
        if (str_ieq(app->cfg.mode, "marathon")) (void)str_snprintf(app->cfg.mode, sizeof(app->cfg.mode), "sprint");
        else if (str_ieq(app->cfg.mode, "sprint")) (void)str_snprintf(app->cfg.mode, sizeof(app->cfg.mode), "ultra");
        else (void)str_snprintf(app->cfg.mode, sizeof(app->cfg.mode), "marathon");
      } else if (s->sel == 1) {
        app->cfg.ghost = app->cfg.ghost ? 0 : 1;
      } else if (s->sel == 2) {
        app->cfg.hold = app->cfg.hold ? 0 : 1;
      } else if (s->sel == 3) {
        app->cfg.das_ms = clamp_i(app->cfg.das_ms + 10, 60, 250);
        input_set_from_config(&app->input, &app->cfg);
      } else if (s->sel == 4) {
        app->cfg.arr_ms = clamp_i(app->cfg.arr_ms + 5, 0, 80);
        input_set_from_config(&app->input, &app->cfg);
      } else if (s->sel == 5) {
        app->cfg.start_level = clamp_i(app->cfg.start_level + 1, 1, 15);
      } else if (s->sel == 6) {
        app->cfg.fullscreen = app->cfg.fullscreen ? 0 : 1;
      }
    }
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  OptionsState* s = (OptionsState*)ss;
  Renderer* rd = &app->renderer;

  draw_text(rd, 20, 20, 3, (ColorRGBA){240,240,255,255}, "OPTIONS");

  char line[128];

  const char* labels[7] = {
    "MODE",
    "GHOST",
    "HOLD",
    "DAS (MS)",
    "ARR (MS)",
    "START LEVEL",
    "FULLSCREEN"
  };

  for (int i = 0; i < 7; ++i) {
    const int y = 80 + i * 24;
    const ColorRGBA c = (i == s->sel) ? (ColorRGBA){255,220,120,255} : (ColorRGBA){210,210,230,255};

    if (i == 0) (void)snprintf(line, sizeof(line), "%s: %s", labels[i], app->cfg.mode);
    else if (i == 1) (void)snprintf(line, sizeof(line), "%s: %s", labels[i], app->cfg.ghost ? "ON" : "OFF");
    else if (i == 2) (void)snprintf(line, sizeof(line), "%s: %s", labels[i], app->cfg.hold ? "ON" : "OFF");
    else if (i == 3) (void)snprintf(line, sizeof(line), "%s: %d", labels[i], app->cfg.das_ms);
    else if (i == 4) (void)snprintf(line, sizeof(line), "%s: %d", labels[i], app->cfg.arr_ms);
    else if (i == 5) (void)snprintf(line, sizeof(line), "%s: %d", labels[i], app->cfg.start_level);
    else (void)snprintf(line, sizeof(line), "%s: %s", labels[i], app->cfg.fullscreen ? "YES" : "NO");

    draw_text(rd, 26, y, 2, c, line);
  }

  draw_text(rd, 20, 260, 1, (ColorRGBA){160,160,180,255}, "ENTER=TOGGLE  BACKSPACE/ESC=BACK (SAVES config.ini)");
}

static void destroy(State* ss) { free(ss); }

State* options_state_create(void) {
  OptionsState* s = (OptionsState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;

  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Options";
  s->sel = 0;
  return (State*)s;
}