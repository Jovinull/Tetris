#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct Config {
  int fullscreen;   // 0/1
  int scale;        // 2..6

  // game options
  char mode[16];    // marathon/sprint/ultra
  int ghost;        // 0/1
  int hold;         // 0/1
  int das_ms;       // 60..250
  int arr_ms;       // 0..80 (0 = instant)
  int start_level;  // 1..15

  // key bindings (string tokens)
  char key_left[16];
  char key_right[16];
  char key_down[16];
  char key_hard_drop[16];
  char key_rot_cw[16];
  char key_rot_ccw[16];
  char key_hold[16];
  char key_pause[16];
  char key_confirm[16];
  char key_back[16];
} Config;

void config_set_defaults(Config* cfg);
bool config_load_ini(Config* cfg, const char* path);
bool config_save_ini(const Config* cfg, const char* path);