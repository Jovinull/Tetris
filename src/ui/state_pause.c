#include "ui/state_pause.h"
#include "core/app.h"
#include <stdlib.h>

typedef struct PauseState { State base; } PauseState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e; (void)ss;
  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (cmd.pressed && (cmd.action == ACT_PAUSE || cmd.action == ACT_BACK || cmd.action == ACT_CONFIRM)) {
      (void)state_stack_pop(&app->states, app);
    }
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  (void)ss;
  Renderer* rd = &app->renderer;
  draw_rect(rd, 0, 0, rd->logical_w, rd->logical_h, (ColorRGBA){0,0,0,140}, true);
  draw_text(rd, 120, 120, 3, (ColorRGBA){240,240,255,255}, "PAUSED");
  draw_text(rd, 110, 160, 1, (ColorRGBA){200,200,220,255}, "P/ENTER/BACKSPACE to resume");
}

static void destroy(State* ss) { free(ss); }

State* pause_state_create(void) {
  PauseState* s = (PauseState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Pause";
  return (State*)s;
}