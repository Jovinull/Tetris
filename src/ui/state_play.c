#include "ui/state_play.h"
#include "core/app.h"
#include "ui/hud.h"
#include "ui/fx.h"
#include "ui/state_pause.h"
#include "ui/state_gameover.h"
#include "utils/str.h"
#include <stdlib.h>
#include <string.h>

typedef struct PlayState {
  State base;
  Gameplay game;
  FxSystem fx;
  Renderer* rd_ref; // para shake via listener
} PlayState;

static void shake_listener(void* user, const GameEvent* ev) {
  Renderer* rd = (Renderer*)user;
  if (!rd || !ev) return;
  if (ev->type == EV_LINE_CLEAR && ev->a == 4) {
    renderer_set_shake(rd, 160, 6);
  }
}

static void on_enter(State* ss, App* app) {
  PlayState* s = (PlayState*)ss;

  GameplayOptions opt;
  memset(&opt, 0, sizeof(opt));
  opt.ghost = (app->cfg.ghost != 0);
  opt.hold_enabled = (app->cfg.hold != 0);
  opt.start_level = app->cfg.start_level;
  (void)str_snprintf(opt.mode, sizeof(opt.mode), "%s", app->cfg.mode);
  opt.lock_delay_ms = 500;
  opt.lock_reset_limit = 15;

  (void)gameplay_init(&s->game, &app->bus, &opt, (uint32_t)SDL_GetTicks());

  fx_init(&s->fx, &app->bus);

  s->rd_ref = &app->renderer;
  (void)event_bus_subscribe(&app->bus, s->rd_ref, shake_listener);
}

static void on_exit(State* ss, App* app) {
  (void)app;
  PlayState* s = (PlayState*)ss;
  gameplay_shutdown(&s->game);
}

static void handle_event(State* ss, App* app, const SDL_Event* e) {
  (void)e;
  PlayState* s = (PlayState*)ss;

  InputCommand cmd;
  while (input_poll(&app->input, &cmd)) {
    if (!cmd.pressed) continue;

    if (cmd.action == ACT_PAUSE) {
      (void)state_stack_push(&app->states, app, pause_state_create());
      return;
    }
    if (cmd.action == ACT_RESTART) {
      gameplay_reset(&s->game, (uint32_t)SDL_GetTicks());
      return;
    }

    gameplay_apply_action(&s->game, cmd.action, cmd.pressed);
  }
}

static void update(State* ss, App* app, double dt) {
  (void)dt;
  PlayState* s = (PlayState*)ss;

  fx_update(&s->fx, 16);
  gameplay_update(&s->game, 16);

  if (s->game.game_over) {
    (void)state_stack_push(&app->states, app, gameover_state_create());
  }
}

static void render(State* ss, App* app) {
  PlayState* s = (PlayState*)ss;
  Renderer* rd = &app->renderer;

  const int cell = 12;
  const int ox = 120;
  const int oy = 24;

  hud_render(rd, &s->game, ox, oy, cell);
  fx_render_overlay(&s->fx, rd);
}

static void destroy(State* ss) { free(ss); }

State* play_state_create(void) {
  PlayState* s = (PlayState*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  static const StateVTable V = { on_enter, on_exit, handle_event, update, render, destroy };
  s->base.v = &V;
  s->base.name = "Play";
  return (State*)s;
}