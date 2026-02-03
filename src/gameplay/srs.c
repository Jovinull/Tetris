#include "gameplay/srs.h"

/*
  Strategy: SRS rotation + kicks.
  dir: +1 CW, -1 CCW
*/

typedef struct Kick { int dx, dy; } Kick;

static const Kick JLSTZ_KICKS[4][5] = {
  // 0 -> R
  {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
  // R -> 2
  {{0,0},{1,0},{1,-1},{0,2},{1,2}},
  // 2 -> L
  {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
  // L -> 0
  {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
};

static const Kick JLSTZ_KICKS_CCW[4][5] = {
  // 0 -> L
  {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
  // R -> 0
  {{0,0},{1,0},{1,-1},{0,2},{1,2}},
  // 2 -> R
  {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
  // L -> 2
  {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
};

static const Kick I_KICKS[4][5] = {
  // 0 -> R
  {{0,0},{-2,0},{1,0},{-2,-1},{1,2}},
  // R -> 2
  {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},
  // 2 -> L
  {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},
  // L -> 0
  {{0,0},{1,0},{-2,0},{1,-2},{-2,1}},
};

static const Kick I_KICKS_CCW[4][5] = {
  // 0 -> L
  {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},
  // R -> 0
  {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},
  // 2 -> R
  {{0,0},{1,0},{-2,0},{1,-2},{-2,1}},
  // L -> 2
  {{0,0},{-2,0},{1,0},{-2,-1},{1,2}},
};

static bool try_kicks(const Board* b, const Piece* rotated, const Kick kicks[5], Piece* out) {
  for (int i = 0; i < 5; ++i) {
    Piece cand = *rotated;
    cand.x += kicks[i].dx;
    cand.y += kicks[i].dy;
    if (board_can_place(b, &cand)) {
      *out = cand;
      return true;
    }
  }
  return false;
}

static bool srs_rotate_impl(const IRotationSystem* rs, const Board* b, const Piece* in, int dir, Piece* out) {
  (void)rs;
  if (!b || !in || !out) return false;

  if (in->t == TET_O) {
    Piece r = piece_rotated(in, dir);
    if (board_can_place(b, &r)) { *out = r; return true; }
    return false;
  }

  const Piece r = piece_rotated(in, dir);
  if (board_can_place(b, &r)) { *out = r; return true; }

  const int from = (int)in->rot;
  if (in->t == TET_I) {
    const Kick (*table)[5] = (dir > 0) ? I_KICKS : I_KICKS_CCW;
    return try_kicks(b, &r, table[from], out);
  } else {
    const Kick (*table)[5] = (dir > 0) ? JLSTZ_KICKS : JLSTZ_KICKS_CCW;
    return try_kicks(b, &r, table[from], out);
  }
}

static const IRotationSystem SRS = { srs_rotate_impl };

const IRotationSystem* srs_system(void) { return &SRS; }