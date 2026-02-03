#include "gameplay/gameplay.h"
#include "utils/str.h"
#include "utils/log.h"
#include <string.h>

static int gravity_for_level(int level) {
  // curva simples e previsível (ms entre quedas)
  // level 1: 800ms, reduz até ~80ms
  const int l = clamp_i(level, 1, 20);
  const int g = 800 - (l - 1) * 40;
  return clamp_i(g, 80, 800);
}

static void fill_next(Gameplay* g) {
  while (g->next_count < 5) {
    g->next[g->next_count++] = g->rand->next(g->rand);
  }
}

static Tetromino pop_next(Gameplay* g) {
  const Tetromino t = g->next[0];
  for (int i = 0; i < 4; ++i) g->next[i] = g->next[i + 1];
  g->next_count--;
  fill_next(g);
  return t;
}

static bool spawn_piece(Gameplay* g) {
  const Tetromino t = pop_next(g);
  piece_spawn(&g->cur, t);
  g->has_cur = true;
  g->hold_used = false;
  g->lock_timer_ms = 0;
  g->lock_resets = 0;

  // spawn em hidden rows (y=0..1) faz parte do board
  if (!board_can_place(&g->board, &g->cur)) {
    g->game_over = true;
    if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_GAME_OVER, 0,0,0 });
    return false;
  }
  if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_PIECE_SPAWN, (int)t, 0, 0 });
  return true;
}

static void lock_piece(Gameplay* g) {
  board_place(&g->board, &g->cur);
  if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_PIECE_LOCK, (int)g->cur.t, 0, 0 });

  int rows[4] = {0,0,0,0};
  const int cleared = board_clear_lines(&g->board, rows);

  const ScoreResult sr = g->score->on_clear(g->score, cleared, false, 0);
  if (cleared > 0 && g->bus) {
    event_bus_publish(g->bus, &(GameEvent){ EV_LINE_CLEAR, cleared, sr.tetris ? 1 : 0, 0 });
  }
  const ScoreState* st_after = g->score->get(g->score);
  if (sr.level_up && g->bus) {
    event_bus_publish(g->bus, &(GameEvent){ EV_LEVEL_UP, st_after->level, 0, 0 });
  }

  g->gravity_ms = gravity_for_level(st_after->level);
  g->has_cur = false;
}

static bool try_move(Gameplay* g, int dx, int dy) {
  Piece p = g->cur;
  p.x += dx;
  p.y += dy;
  if (board_can_place(&g->board, &p)) {
    g->cur = p;
    if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_PIECE_MOVE, dx, dy, 0 });
    return true;
  }
  return false;
}

static bool on_ground(const Gameplay* g) {
  Piece p = g->cur;
  p.y += 1;
  return !board_can_place(&g->board, &p);
}

static void reset_lock_delay(Gameplay* g) {
  if (!on_ground(g)) return;
  if (g->lock_resets >= g->opt.lock_reset_limit) return;
  g->lock_timer_ms = 0;
  g->lock_resets += 1;
}

static void try_rotate(Gameplay* g, int dir) {
  Piece out;
  if (g->rot->rotate(g->rot, &g->board, &g->cur, dir, &out)) {
    g->cur = out;
    reset_lock_delay(g);
    if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_PIECE_ROTATE, dir, 0, 0 });
  }
}

static void do_hold(Gameplay* g) {
  if (!g->opt.hold_enabled) return;
  if (g->hold_used) return;
  g->hold_used = true;

  const Tetromino cur = g->cur.t;
  if (!g->has_hold) {
    g->hold = cur;
    g->has_hold = true;
    (void)spawn_piece(g);
  } else {
    const Tetromino tmp = g->hold;
    g->hold = cur;
    piece_spawn(&g->cur, tmp);
    if (!board_can_place(&g->board, &g->cur)) {
      g->game_over = true;
      if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_GAME_OVER, 0,0,0 });
      return;
    }
  }

  if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_HOLD, (int)g->hold, 0, 0 });
}

static void hard_drop(Gameplay* g) {
  const int dist = board_drop_distance(&g->board, &g->cur);
  g->cur.y += dist;
  // pontuação de hard drop
  (void)g->score->on_clear(g->score, 0, true, dist);
  lock_piece(g);
  (void)spawn_piece(g);
}

bool gameplay_init(Gameplay* g, EventBus* bus, const GameplayOptions* opt, uint32_t seed) {
  if (!g || !opt) return false;
  memset(g, 0, sizeof(*g));
  g->bus = bus;
  g->rot = srs_system();
  g->rand = randomizer_7bag_create();
  g->score = scoring_modern_lite_create();
  if (!g->rand || !g->score) return false;

  g->opt = *opt;
  g->opt.lock_delay_ms = clamp_i(g->opt.lock_delay_ms, 100, 1000);
  g->opt.lock_reset_limit = clamp_i(g->opt.lock_reset_limit, 0, 30);
  (void)str_snprintf(g->opt.mode, sizeof(g->opt.mode), "%s", opt->mode);

  gameplay_reset(g, seed);
  return true;
}

void gameplay_shutdown(Gameplay* g) {
  if (!g) return;
  if (g->rand) { g->rand->destroy(g->rand); g->rand = NULL; }
  if (g->score) { g->score->destroy(g->score); g->score = NULL; }
}

void gameplay_reset(Gameplay* g, uint32_t seed) {
  if (!g) return;
  board_clear(&g->board);
  g->has_cur = false;
  g->next_count = 0;
  g->has_hold = false;
  g->hold_used = false;
  g->paused = false;
  g->game_over = false;
  g->elapsed_ms = 0;
  g->lock_timer_ms = 0;
  g->lock_resets = 0;

  g->rand->reset(g->rand, seed);
  g->score->reset(g->score, g->opt.start_level);

  fill_next(g);
  (void)spawn_piece(g);

  const ScoreState* st = g->score->get(g->score);
  g->gravity_ms = gravity_for_level(st->level);

  // modos
  g->lines_target = 40;
  g->time_limit_ms = 120000; // 2min ultra-lite
}

void gameplay_apply_action(Gameplay* g, GameAction act, bool pressed) {
  if (!g) return;
  if (!pressed) return; // ações são “on press” (repeat vem do input_update)
  if (g->game_over) return;
  if (g->paused && act != ACT_PAUSE) return;

  switch (act) {
    case ACT_LEFT:  if (try_move(g, -1, 0)) reset_lock_delay(g); break;
    case ACT_RIGHT: if (try_move(g, 1, 0)) reset_lock_delay(g); break;
    case ACT_DOWN:  (void)try_move(g, 0, 1); break; // soft drop
    case ACT_ROT_CW:  try_rotate(g, +1); break;
    case ACT_ROT_CCW: try_rotate(g, -1); break;
    case ACT_HARD_DROP: hard_drop(g); break;
    case ACT_HOLD: do_hold(g); break;
    case ACT_PAUSE:
      g->paused = !g->paused;
      if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_PAUSE_TOGGLE, g->paused ? 1 : 0, 0, 0 });
      break;
    default: break;
  }
}

static void gravity_step(Gameplay* g) {
  if (!g->has_cur) return;
  (void)try_move(g, 0, 1);
}

void gameplay_update(Gameplay* g, int dt_ms) {
  if (!g) return;
  if (g->game_over || g->paused) return;

  g->elapsed_ms += dt_ms;

  // condições de vitória/fim por modo
  const ScoreState* st = g->score->get(g->score);
  if (str_ieq(g->opt.mode, "sprint")) {
    if (st->lines_total >= g->lines_target) {
      g->game_over = true;
      if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_GAME_OVER, 1, 0, 0 }); // 1 = win
      return;
    }
  } else if (str_ieq(g->opt.mode, "ultra")) {
    if (g->elapsed_ms >= g->time_limit_ms) {
      g->game_over = true;
      if (g->bus) event_bus_publish(g->bus, &(GameEvent){ EV_GAME_OVER, 2, 0, 0 }); // 2 = time
      return;
    }
  }

  // tick de gravidade
  g->tick_ms += dt_ms;
  while (g->tick_ms >= g->gravity_ms) {
    g->tick_ms -= g->gravity_ms;
    gravity_step(g);
    if (g->game_over) break;
  }

  if (g->has_cur) {
    if (on_ground(g)) {
      g->lock_timer_ms += dt_ms;
      if (g->lock_timer_ms >= g->opt.lock_delay_ms) {
        lock_piece(g);
        if (!g->game_over) (void)spawn_piece(g);
      }
    } else {
      g->lock_timer_ms = 0;
      g->lock_resets = 0;
    }
  }
}

int gameplay_get_ghost_drop(const Gameplay* g) {
  if (!g || !g->opt.ghost || !g->has_cur) return 0;
  return board_drop_distance(&g->board, &g->cur);
}