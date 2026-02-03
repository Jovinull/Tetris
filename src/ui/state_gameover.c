#include "ui/state_gameover.h"
#include "core/app.h"
#include <stdlib.h>

typedef struct GameOverState { State base; } GameOverState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e; (void)ss;
  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (!cmd.pressed) continue;
    if (cmd.action == ACT_CONFIRM || cmd.action == ACT_BACK) {
      // fecha gameover e play (volta pro menu)
      (void)state_stack_pop(&app->states, app); // gameover
      (void)state_stack_pop(&app->states, app); // play
      return;
    }
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  (void)ss;
  Renderer* rd = &app->renderer;
  draw_rect(rd, 0, 0, rd->logical_w, rd->logical_h, (ColorRGBA){0,0,0,170}, true);
  draw_text(rd, 86, 110, 3, (ColorRGBA){255,170,170,255}, "GAME OVER");
  draw_text(rd, 70, 160, 1, (ColorRGBA){220,220,240,255}, "ENTER/BACKSPACE to return to menu");
}

static void destroy(State* ss) { free(ss); }

State* gameover_state_create(void) {
  GameOverState* s = (GameOverState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "GameOver";
  return (State*)s;
}