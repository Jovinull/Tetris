#include "ui/state_controls.h"
#include "core/app.h"
#include <stdlib.h>

typedef struct ControlsState {
  State base;
} ControlsState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e; (void)ss;
  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (cmd.pressed && cmd.action == ACT_BACK) {
      (void)state_stack_pop(&app->states, app);
    }
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  (void)ss;
  Renderer* rd = &app->renderer;

  draw_text(rd, 20, 20, 3, (ColorRGBA){240,240,255,255}, "CONTROLS");
  draw_text(rd, 20, 80, 2, (ColorRGBA){210,210,230,255},
    "MOVE: LEFT/RIGHT or A/D\n"
    "SOFT DROP: DOWN or S\n"
    "HARD DROP: SPACE\n"
    "ROTATE: Z (CCW) / X (CW)\n"
    "HOLD: C / LSHIFT\n"
    "PAUSE: P / ESC\n"
    "RESTART: R\n"
  );
  draw_text(rd, 20, 260, 1, (ColorRGBA){160,160,180,255}, "BACKSPACE=BACK");
}

static void destroy(State* ss) { free(ss); }

State* controls_state_create(void) {
  ControlsState* s = (ControlsState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Controls";
  return (State*)s;
}