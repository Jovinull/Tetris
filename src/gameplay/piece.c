#include "gameplay/piece.h"

/*
  Definição de 4 blocos por rotação.
  Coordenadas relativas ao pivot (0,0). Convenção semelhante a SRS (pivot por peça).
  O e I têm pivôs especiais, mas a rotação final é ajustada pelos kicks do SRS.
*/

static void cells_I(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){2,0}; break;
    case ROT_R: out[0]=(Cell){1,-1}; out[1]=(Cell){1,0}; out[2]=(Cell){1,1}; out[3]=(Cell){1,2}; break;
    case ROT_2: out[0]=(Cell){-1,1}; out[1]=(Cell){0,1}; out[2]=(Cell){1,1}; out[3]=(Cell){2,1}; break;
    case ROT_L: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){0,2}; break;
  }
}

static void cells_O(Rot r, Cell out[4]) {
  (void)r;
  out[0]=(Cell){0,0}; out[1]=(Cell){1,0}; out[2]=(Cell){0,1}; out[3]=(Cell){1,1};
}

static void cells_T(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){0,1}; break;
    case ROT_R: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){1,0}; break;
    case ROT_2: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){0,-1}; break;
    case ROT_L: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){-1,0}; break;
  }
}

static void cells_S(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){1,1}; break;
    case ROT_R: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){1,1}; break;
    case ROT_2: out[0]=(Cell){-1,-1}; out[1]=(Cell){0,-1}; out[2]=(Cell){0,0}; out[3]=(Cell){1,0}; break;
    case ROT_L: out[0]=(Cell){-1,-1}; out[1]=(Cell){-1,0}; out[2]=(Cell){0,0}; out[3]=(Cell){0,1}; break;
  }
}

static void cells_Z(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,1}; out[1]=(Cell){0,1}; out[2]=(Cell){0,0}; out[3]=(Cell){1,0}; break;
    case ROT_R: out[0]=(Cell){1,-1}; out[1]=(Cell){1,0}; out[2]=(Cell){0,0}; out[3]=(Cell){0,1}; break;
    case ROT_2: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){0,-1}; out[3]=(Cell){1,-1}; break;
    case ROT_L: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){-1,0}; out[3]=(Cell){-1,1}; break;
  }
}

static void cells_J(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){-1,1}; break;
    case ROT_R: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){1,1}; break;
    case ROT_2: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){1,-1}; break;
    case ROT_L: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){-1,-1}; break;
  }
}

static void cells_L(Rot r, Cell out[4]) {
  switch (r) {
    case ROT_0: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){1,1}; break;
    case ROT_R: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){1,-1}; break;
    case ROT_2: out[0]=(Cell){-1,0}; out[1]=(Cell){0,0}; out[2]=(Cell){1,0}; out[3]=(Cell){-1,-1}; break;
    case ROT_L: out[0]=(Cell){0,-1}; out[1]=(Cell){0,0}; out[2]=(Cell){0,1}; out[3]=(Cell){-1,1}; break;
  }
}

void piece_cells_at(Tetromino t, Rot r, int px, int py, Cell out[4]) {
  Cell rel[4];
  switch (t) {
    case TET_I: cells_I(r, rel); break;
    case TET_O: cells_O(r, rel); break;
    case TET_T: cells_T(r, rel); break;
    case TET_S: cells_S(r, rel); break;
    case TET_Z: cells_Z(r, rel); break;
    case TET_J: cells_J(r, rel); break;
    case TET_L: cells_L(r, rel); break;
    default:    cells_T(r, rel); break;
  }
  for (int i = 0; i < 4; ++i) {
    out[i].x = px + rel[i].x;
    out[i].y = py + rel[i].y;
  }
}

void piece_spawn(Piece* p, Tetromino t) {
  if (!p) return;
  p->t = t;
  p->rot = ROT_0;
  p->x = 4;   // spawn center-ish for 10 wide
  p->y = 0;   // inclui hidden rows no board
}

void piece_cells(const Piece* p, Cell out[4]) {
  if (!p || !out) return;
  piece_cells_at(p->t, p->rot, p->x, p->y, out);
}

Piece piece_rotated(const Piece* p, int dir) {
  Piece r = *p;
  int v = (int)p->rot + dir;
  while (v < 0) v += 4;
  while (v > 3) v -= 4;
  r.rot = (Rot)v;
  return r;
}