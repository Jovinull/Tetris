#include "utils/config.h"
#include "utils/str.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void trim(char* s) {
  if (!s) return;
  size_t n = strlen(s);
  while (n > 0u && isspace((unsigned char)s[n - 1u])) s[--n] = '\0';
  size_t i = 0u;
  while (s[i] && isspace((unsigned char)s[i])) i++;
  if (i > 0u) memmove(s, s + i, strlen(s + i) + 1u);
}

void config_set_defaults(Config* c) {
  if (!c) return;
  c->fullscreen = 0;
  c->scale = 3;

  (void)str_snprintf(c->mode, sizeof(c->mode), "marathon");
  c->ghost = 1;
  c->hold = 1;
  c->das_ms = 140;
  c->arr_ms = 25;
  c->start_level = 1;

  (void)str_snprintf(c->key_left, sizeof(c->key_left), "A");
  (void)str_snprintf(c->key_right, sizeof(c->key_right), "D");
  (void)str_snprintf(c->key_down, sizeof(c->key_down), "S");
  (void)str_snprintf(c->key_hard_drop, sizeof(c->key_hard_drop), "SPACE");
  (void)str_snprintf(c->key_rot_cw, sizeof(c->key_rot_cw), "X");
  (void)str_snprintf(c->key_rot_ccw, sizeof(c->key_rot_ccw), "Z");
  (void)str_snprintf(c->key_hold, sizeof(c->key_hold), "C");
  (void)str_snprintf(c->key_pause, sizeof(c->key_pause), "ESC");
  (void)str_snprintf(c->key_confirm, sizeof(c->key_confirm), "RETURN");
  (void)str_snprintf(c->key_back, sizeof(c->key_back), "BACKSPACE");
}

static bool parse_int(const char* v, int* out) {
  if (!v || !out) return false;
  int sign = 1;
  size_t i = 0u;
  if (v[0] == '-') { sign = -1; i = 1u; }
  int acc = 0;
  bool any = false;
  for (; v[i]; ++i) {
    if (!isdigit((unsigned char)v[i])) return false;
    any = true;
    acc = acc * 10 + (v[i] - '0');
  }
  if (!any) return false;
  *out = acc * sign;
  return true;
}

static void set_key(char* dst, size_t dst_sz, const char* v) {
  if (!dst || dst_sz == 0u || !v) return;
  (void)str_snprintf(dst, dst_sz, "%s", v);
}

static void apply_kv(Config* c, const char* section, const char* k, const char* v) {
  if (!c || !section || !k || !v) return;

  if (str_ieq(section, "video")) {
    if (str_ieq(k, "fullscreen")) { int x; if (parse_int(v, &x)) c->fullscreen = (x != 0); }
    if (str_ieq(k, "scale"))      { int x; if (parse_int(v, &x)) c->scale = clamp_i(x, 2, 6); }
  } else if (str_ieq(section, "game")) {
    if (str_ieq(k, "mode"))       set_key(c->mode, sizeof(c->mode), v);
    if (str_ieq(k, "ghost"))      { int x; if (parse_int(v, &x)) c->ghost = (x != 0); }
    if (str_ieq(k, "hold"))       { int x; if (parse_int(v, &x)) c->hold = (x != 0); }
    if (str_ieq(k, "das_ms"))     { int x; if (parse_int(v, &x)) c->das_ms = clamp_i(x, 60, 250); }
    if (str_ieq(k, "arr_ms"))     { int x; if (parse_int(v, &x)) c->arr_ms = clamp_i(x, 0, 80); }
    if (str_ieq(k, "start_level")){ int x; if (parse_int(v, &x)) c->start_level = clamp_i(x, 1, 15); }
  } else if (str_ieq(section, "keys")) {
    if (str_ieq(k, "left"))       set_key(c->key_left, sizeof(c->key_left), v);
    if (str_ieq(k, "right"))      set_key(c->key_right, sizeof(c->key_right), v);
    if (str_ieq(k, "down"))       set_key(c->key_down, sizeof(c->key_down), v);
    if (str_ieq(k, "hard_drop"))  set_key(c->key_hard_drop, sizeof(c->key_hard_drop), v);
    if (str_ieq(k, "rot_cw"))     set_key(c->key_rot_cw, sizeof(c->key_rot_cw), v);
    if (str_ieq(k, "rot_ccw"))    set_key(c->key_rot_ccw, sizeof(c->key_rot_ccw), v);
    if (str_ieq(k, "hold"))       set_key(c->key_hold, sizeof(c->key_hold), v);
    if (str_ieq(k, "pause"))      set_key(c->key_pause, sizeof(c->key_pause), v);
    if (str_ieq(k, "confirm"))    set_key(c->key_confirm, sizeof(c->key_confirm), v);
    if (str_ieq(k, "back"))       set_key(c->key_back, sizeof(c->key_back), v);
  }
}

bool config_load_ini(Config* c, const char* path) {
  if (!c || !path) return false;
  FILE* f = fopen(path, "rb");
  if (!f) return false;

  char section[32] = "video";
  char line[256];

  while (fgets(line, (int)sizeof(line), f)) {
    trim(line);
    if (line[0] == '\0' || line[0] == ';' || line[0] == '#') continue;

    if (line[0] == '[') {
      char* end = strchr(line, ']');
      if (!end) continue;
      *end = '\0';
      (void)str_snprintf(section, sizeof(section), "%s", line + 1);
      trim(section);
      continue;
    }

    char* eq = strchr(line, '=');
    if (!eq) continue;
    *eq = '\0';
    char* k = line;
    char* v = eq + 1;
    trim(k); trim(v);
    if (*k == '\0') continue;

    apply_kv(c, section, k, v);
  }

  (void)fclose(f);
  return true;
}

bool config_save_ini(const Config* c, const char* path) {
  if (!c || !path) return false;
  FILE* f = fopen(path, "wb");
  if (!f) return false;

  fprintf(f,
    "[video]\n"
    "fullscreen=%d\n"
    "scale=%d\n\n"
    "[game]\n"
    "mode=%s\n"
    "ghost=%d\n"
    "hold=%d\n"
    "das_ms=%d\n"
    "arr_ms=%d\n"
    "start_level=%d\n\n"
    "[keys]\n"
    "left=%s\n"
    "right=%s\n"
    "down=%s\n"
    "hard_drop=%s\n"
    "rot_cw=%s\n"
    "rot_ccw=%s\n"
    "hold=%s\n"
    "pause=%s\n"
    "confirm=%s\n"
    "back=%s\n",
    c->fullscreen, c->scale,
    c->mode, c->ghost, c->hold, c->das_ms, c->arr_ms, c->start_level,
    c->key_left, c->key_right, c->key_down, c->key_hard_drop,
    c->key_rot_cw, c->key_rot_ccw, c->key_hold, c->key_pause,
    c->key_confirm, c->key_back
  );

  (void)fclose(f);
  return true;
}