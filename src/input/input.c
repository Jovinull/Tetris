#include "input/input.h"
#include "utils/str.h"
#include <string.h>

static SDL_Scancode sc_from_token(const char* t) {
  if (!t || t[0] == '\0') return SDL_SCANCODE_UNKNOWN;

  // letras
  if (t[1] == '\0' && ((t[0] >= 'A' && t[0] <= 'Z') || (t[0] >= 'a' && t[0] <= 'z'))) {
    const char up = (t[0] >= 'a') ? (char)(t[0] - 32) : t[0];
    return (SDL_Scancode)(SDL_SCANCODE_A + (up - 'A'));
  }

  if (str_ieq(t, "LEFT")) return SDL_SCANCODE_LEFT;
  if (str_ieq(t, "RIGHT")) return SDL_SCANCODE_RIGHT;
  if (str_ieq(t, "DOWN")) return SDL_SCANCODE_DOWN;
  if (str_ieq(t, "UP")) return SDL_SCANCODE_UP;
  if (str_ieq(t, "SPACE")) return SDL_SCANCODE_SPACE;
  if (str_ieq(t, "ESC") || str_ieq(t, "ESCAPE")) return SDL_SCANCODE_ESCAPE;
  if (str_ieq(t, "RETURN") || str_ieq(t, "ENTER")) return SDL_SCANCODE_RETURN;
  if (str_ieq(t, "BACKSPACE")) return SDL_SCANCODE_BACKSPACE;
  if (str_ieq(t, "LSHIFT")) return SDL_SCANCODE_LSHIFT;

  return SDL_SCANCODE_UNKNOWN;
}

static void q_push(Input* in, GameAction a, bool pressed) {
  if (!in || a == ACT_NONE) return;
  const int next = (in->q_tail + 1) & 31;
  if (next == in->q_head) return; // drop se cheio (robusto)
  in->queue[in->q_tail] = (InputCommand){ a, pressed };
  in->q_tail = next;
}

bool input_poll(Input* in, InputCommand* out) {
  if (!in || !out) return false;
  if (in->q_head == in->q_tail) return false;
  *out = in->queue[in->q_head];
  in->q_head = (in->q_head + 1) & 31;
  return true;
}

void input_set_from_config(Input* in, const Config* cfg) {
  if (!in || !cfg) return;

  in->map[ACT_LEFT]      = sc_from_token(cfg->key_left);
  in->map[ACT_RIGHT]     = sc_from_token(cfg->key_right);
  in->map[ACT_DOWN]      = sc_from_token(cfg->key_down);
  in->map[ACT_HARD_DROP] = sc_from_token(cfg->key_hard_drop);
  in->map[ACT_ROT_CW]    = sc_from_token(cfg->key_rot_cw);
  in->map[ACT_ROT_CCW]   = sc_from_token(cfg->key_rot_ccw);
  in->map[ACT_HOLD]      = sc_from_token(cfg->key_hold);
  in->map[ACT_PAUSE]     = sc_from_token(cfg->key_pause);
  in->map[ACT_CONFIRM]   = sc_from_token(cfg->key_confirm);
  in->map[ACT_BACK]      = sc_from_token(cfg->key_back);

  in->das_ms = cfg->das_ms;
  in->arr_ms = cfg->arr_ms;
}

void input_init(Input* in, const Config* cfg) {
  if (!in) return;
  memset(in, 0, sizeof(*in));
  for (int i = 0; i <= ACT_RESTART; ++i) in->map[i] = SDL_SCANCODE_UNKNOWN;
  input_set_from_config(in, cfg);
}

static GameAction action_from_scancode(const Input* in, SDL_Scancode sc) {
  if (!in) return ACT_NONE;
  for (int a = 1; a <= ACT_RESTART; ++a) {
    if (in->map[a] == sc) return (GameAction)a;
  }

  // hardcoded extras
  if (sc == SDL_SCANCODE_LEFT)  return ACT_LEFT;
  if (sc == SDL_SCANCODE_RIGHT) return ACT_RIGHT;
  if (sc == SDL_SCANCODE_DOWN)  return ACT_DOWN;
  if (sc == SDL_SCANCODE_UP) return ACT_UP;
  if (sc == SDL_SCANCODE_W)  return ACT_UP;
  if (sc == SDL_SCANCODE_S)  return ACT_DOWN;
  if (sc == SDL_SCANCODE_P) return ACT_PAUSE;
  if (sc == SDL_SCANCODE_R) return ACT_RESTART;
  return ACT_NONE;
}

void input_handle_event(Input* in, const SDL_Event* e) {
  if (!in || !e) return;
  if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
    const bool pressed = (e->type == SDL_KEYDOWN);
    const SDL_Scancode sc = e->key.keysym.scancode;

    const GameAction a = action_from_scancode(in, sc);
    if (a != ACT_NONE) {
      // Debounce automático: ignora repeats do SDL, nosso repeat é próprio
      if (pressed && e->key.repeat) return;

      // rastrear held para left/right
      if (a == ACT_LEFT)  { in->left_down  = pressed; if (!pressed) in->left_held_ms = 0; }
      if (a == ACT_RIGHT) { in->right_down = pressed; if (!pressed) in->right_held_ms = 0; }
      if (a == ACT_DOWN)  { in->down_down  = pressed; }

      q_push(in, a, pressed);
    }
  }
}

static void process_das_arr(Input* in, int dt_ms) {
  if (!in) return;

  const int das = clamp_i(in->das_ms, 60, 250);
  const int arr = clamp_i(in->arr_ms, 0, 80);

  if (in->left_down && !in->right_down) {
    in->left_held_ms += dt_ms;
    if (in->left_held_ms >= das) {
      if (arr == 0) {
        // instant: empurra “muitos” moves; gameplay clampa por colisão
        q_push(in, ACT_LEFT, true);
      } else {
        const int repeats = in->left_held_ms / arr;
        if (repeats > 0) {
          q_push(in, ACT_LEFT, true);
          in->left_held_ms = das; // mantém na borda do repeat
        }
      }
    }
  } else {
    in->left_held_ms = 0;
  }

  if (in->right_down && !in->left_down) {
    in->right_held_ms += dt_ms;
    if (in->right_held_ms >= das) {
      if (arr == 0) {
        q_push(in, ACT_RIGHT, true);
      } else {
        const int repeats = in->right_held_ms / arr;
        if (repeats > 0) {
          q_push(in, ACT_RIGHT, true);
          in->right_held_ms = das;
        }
      }
    }
  } else {
    in->right_held_ms = 0;
  }
}

void input_update(Input* in, int dt_ms) {
  if (!in) return;
  process_das_arr(in, dt_ms);

}

bool input_is_down(const Input* in, GameAction a) {
  if (!in) return false;
  switch (a) {
    case ACT_LEFT:  return in->left_down;
    case ACT_RIGHT: return in->right_down;
    case ACT_DOWN:  return in->down_down;
    default: return false;
  }
}
