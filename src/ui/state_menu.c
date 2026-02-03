#include "ui/state_menu.h"
#include "core/app.h"
#include "render/renderer.h"
#include "utils/str.h"
#include "ui/state_play.h"
#include "ui/state_options.h"
#include "ui/state_controls.h"
#include "ui/state_credits.h"
#include <stdlib.h>
#include <string.h>

typedef struct MenuState {
  State base;
  int sel;
  const char* items[5];
  int count;
} MenuState;

static void on_enter(State* s, App* app) { (void)s; (void)app; }
static void on_exit(State* s, App* app)  { (void)s; (void)app; }

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e;
  MenuState* s = (MenuState*)ss;

  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (!cmd.pressed) continue;

    if (cmd.action == ACT_DOWN) s->sel = (s->sel + 1) % s->count;
    if (cmd.action == ACT_UP) s->sel = (s->sel - 1 + s->count) % s->count;
    if (cmd.action == ACT_CONFIRM) {
      if (s->sel == 0) (void)state_stack_push(&app->states, app, play_state_create());
      if (s->sel == 1) (void)state_stack_push(&app->states, app, options_state_create());
      if (s->sel == 2) (void)state_stack_push(&app->states, app, controls_state_create());
      if (s->sel == 3) (void)state_stack_push(&app->states, app, credits_state_create());
      if (s->sel == 4) app_quit(app);
    }
  }
}

static void update(State* s, App* app, double dt) { (void)s; (void)app; (void)dt; }

static void render(State* ss, App* app) {
  MenuState* s = (MenuState*)ss;
  Renderer* rd = &app->renderer;

  draw_text(rd, 20, 20, 3, (ColorRGBA){240,240,255,255}, "TETRIS");

  for (int i = 0; i < s->count; ++i) {
    const int y = 80 + i * 26;
    const ColorRGBA c = (i == s->sel) ? (ColorRGBA){255,220,120,255} : (ColorRGBA){210,210,230,255};
    draw_text(rd, 34, y, 2, c, s->items[i]);
  }

  draw_text(rd, 20, 250, 1, (ColorRGBA){160,160,180,255}, "ENTER=CONFIRM  BACKSPACE=BACK  P/ESC=PAUSE");
}

static void destroy(State* ss) { free(ss); }

State* menu_state_create(void) {
  MenuState* s = (MenuState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;

  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Menu";

  s->items[0] = "PLAY";
  s->items[1] = "OPTIONS";
  s->items[2] = "CONTROLS";
  s->items[3] = "CREDITS";
  s->items[4] = "QUIT";
  s->count = 5;
  s->sel = 0;
  return (State*)s;
}