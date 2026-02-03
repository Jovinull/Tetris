#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "utils/config.h"

typedef enum GameAction {
  ACT_NONE = 0,
  ACT_LEFT,
  ACT_RIGHT,
  ACT_UP,
  ACT_DOWN,
  ACT_HARD_DROP,
  ACT_ROT_CW,
  ACT_ROT_CCW,
  ACT_HOLD,
  ACT_PAUSE,
  ACT_CONFIRM,
  ACT_BACK,
  ACT_RESTART
} GameAction;

typedef struct InputCommand {
  GameAction action;
  bool pressed;
} InputCommand;

typedef struct Input {
  SDL_Scancode map[ACT_RESTART + 1];

  // DAS/ARR para left/right
  int das_ms;
  int arr_ms;

  int left_held_ms;
  int right_held_ms;
  bool left_down;
  bool right_down;

  // soft drop repeat simples
  bool down_down;

  // queue de comandos
  InputCommand queue[32];
  int q_head;
  int q_tail;
} Input;

void input_init(Input* in, const Config* cfg);
void input_set_from_config(Input* in, const Config* cfg);

void input_handle_event(Input* in, const SDL_Event* e);
void input_update(Input* in, int dt_ms);

bool input_poll(Input* in, InputCommand* out);