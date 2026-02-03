#include "ui/state_credits.h"
#include "core/app.h"
#include <stdlib.h>

typedef struct CreditsState { State base; } CreditsState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e; (void)ss;
  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (cmd.pressed && cmd.action == ACT_BACK) (void)state_stack_pop(&app->states, app);
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  (void)ss;
  Renderer* rd = &app->renderer;
  draw_text(rd, 20, 20, 3, (ColorRGBA){240,240,255,255}, "CREDITS");
  draw_text(rd, 20, 90, 2, (ColorRGBA){210,210,230,255},
    "Tetris C11 (SDL2)\n"
    "Arquitetura modular + testes\n"
    "Feito para estudo e portfólio.\n"
  );
  draw_text(rd, 20, 260, 1, (ColorRGBA){160,160,180,255}, "BACKSPACE=BACK");
}

static void destroy(State* ss) { free(ss); }

State* credits_state_create(void) {
  CreditsState* s = (CreditsState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Credits";
  return (State*)s;
}