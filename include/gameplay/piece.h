#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum Tetromino {
  TET_I = 0,
  TET_O,
  TET_T,
  TET_S,
  TET_Z,
  TET_J,
  TET_L,
  TET_COUNT
} Tetromino;

typedef enum Rot {
  ROT_0 = 0,
  ROT_R = 1,
  ROT_2 = 2,
  ROT_L = 3
} Rot;

typedef struct Piece {
  Tetromino t;
  Rot rot;
  int x; // posição do pivot no grid
  int y;
} Piece;

typedef struct Cell {
  int x;
  int y;
} Cell;

void piece_spawn(Piece* p, Tetromino t);
void piece_cells(const Piece* p, Cell out[4]);
void piece_cells_at(Tetromino t, Rot r, int px, int py, Cell out[4]);

Piece piece_rotated(const Piece* p, int dir); // dir: +1 CW, -1 CCW