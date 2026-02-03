#include "gameplay/scoring.h"
#include "utils/str.h"
#include <stdlib.h>

typedef struct ScoreImpl {
  IScoring api;
  ScoreState st;
  bool modern;
} ScoreImpl;

static void reset_impl(IScoring* ss, int start_level) {
  ScoreImpl* s = (ScoreImpl*)ss;
  s->st.score = 0;
  s->st.level = clamp_i(start_level, 1, 15);
  s->st.lines_total = 0;
  s->st.combo = -1;
  s->st.b2b = 0;
}

static const ScoreState* get_impl(const IScoring* ss) {
  const ScoreImpl* s = (const ScoreImpl*)ss;
  return &s->st;
}

static int base_points(int cleared) {
  switch (cleared) {
    case 1: return 100;
    case 2: return 300;
    case 3: return 500;
    case 4: return 800;
    default: return 0;
  }
}

static ScoreResult on_clear_impl(IScoring* ss, int cleared, bool hard_drop, int drop_dist) {
  ScoreImpl* s = (ScoreImpl*)ss;
  ScoreResult r = {0, cleared, cleared == 4, false};

  // drop points
  if (drop_dist > 0) {
    if (hard_drop) r.add_score += drop_dist * 2;
    else r.add_score += drop_dist;
  }

  if (cleared <= 0) {
    // quebra combo
    s->st.combo = -1;
    return r;
  }

  s->st.lines_total += cleared;
  s->st.combo += 1;

  int pts = base_points(cleared) * s->st.level;

  // modern-lite: b2b e combo
  if (s->modern) {
    const bool is_b2b_candidate = (cleared == 4);
    if (is_b2b_candidate && s->st.b2b > 0) {
      pts = (pts * 3) / 2;
    }
    if (is_b2b_candidate) s->st.b2b += 1;
    else s->st.b2b = 0;

    if (s->st.combo >= 1) pts += 50 * s->st.combo * s->st.level;
  }

  r.add_score += pts;

  // level up a cada 10 linhas
  const int new_level = clamp_i(1 + (s->st.lines_total / 10), 1, 20);
  if (new_level != s->st.level) {
    s->st.level = new_level;
    r.level_up = true;
  }

  s->st.score += r.add_score;
  return r;
}

static void destroy_impl(IScoring* ss) { free(ss); }

IScoring* scoring_classic_create(void) {
  ScoreImpl* s = (ScoreImpl*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  s->modern = false;
  s->api.on_clear = on_clear_impl;
  s->api.reset = reset_impl;
  s->api.get = get_impl;
  s->api.destroy = destroy_impl;
  reset_impl((IScoring*)s, 1);
  return (IScoring*)s;
}

IScoring* scoring_modern_lite_create(void) {
  ScoreImpl* s = (ScoreImpl*)calloc(1u, sizeof(*s));
  if (!s) return NULL;
  s->modern = true;
  s->api.on_clear = on_clear_impl;
  s->api.reset = reset_impl;
  s->api.get = get_impl;
  s->api.destroy = destroy_impl;
  reset_impl((IScoring*)s, 1);
  return (IScoring*)s;
}