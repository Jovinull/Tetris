#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "core/event_bus.h"
#include "gameplay/board.h"
#include "gameplay/randomizer.h"
#include "gameplay/srs.h"
#include "gameplay/scoring.h"
#include "input/input.h"

typedef struct GameplayOptions {
  bool ghost;
  bool hold_enabled;
  int start_level;
  char mode[16]; // marathon/sprint/ultra
  int lock_delay_ms;     // ex: 500
  int lock_reset_limit;  // ex: 15
} GameplayOptions;

typedef struct Gameplay {
  Board board;
  Piece cur;
  bool has_cur;

  Tetromino next[5];
  int next_count;

  Tetromino hold;
  bool has_hold;
  bool hold_used;

  int tick_ms;
  int gravity_ms; // tempo entre quedas por level

  int lock_timer_ms;
  int lock_resets;

  int lines_target; // sprint
  int time_limit_ms; // ultra-lite
  int elapsed_ms;

  bool paused;
  bool game_over;

  GameplayOptions opt;

  const IRotationSystem* rot;
  IRandomizer* rand;
  IScoring* score;

  EventBus* bus;
} Gameplay;

bool gameplay_init(Gameplay* g, EventBus* bus, const GameplayOptions* opt, uint32_t seed);
void gameplay_shutdown(Gameplay* g);

void gameplay_reset(Gameplay* g, uint32_t seed);

void gameplay_apply_action(Gameplay* g, GameAction act, bool pressed);
void gameplay_update(Gameplay* g, int dt_ms);

int gameplay_get_ghost_drop(const Gameplay* g);