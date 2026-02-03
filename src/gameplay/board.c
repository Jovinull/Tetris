#include "gameplay/board.h"
#include <string.h>

void board_clear(Board* b) {
  if (!b) return;
  memset(b->cells, 0, sizeof(b->cells));
}

bool board_in_bounds(int x, int y) {
  return x >= 0 && x < BOARD_W && y >= 0 && y < BOARD_H;
}

bool board_cell_filled(const Board* b, int x, int y) {
  if (!b) return false;
  if (!board_in_bounds(x, y)) return true; // fora é sólido (colisão)
  return b->cells[y][x] != 0u;
}

bool board_can_place(const Board* b, const Piece* p) {
  if (!b || !p) return false;
  Cell c[4];
  piece_cells(p, c);
  for (int i = 0; i < 4; ++i) {
    const int x = c[i].x;
    const int y = c[i].y;
    if (!board_in_bounds(x, y)) return false;
    if (b->cells[y][x] != 0u) return false;
  }
  return true;
}

void board_place(Board* b, const Piece* p) {
  if (!b || !p) return;
  Cell c[4];
  piece_cells(p, c);
  const uint8_t v = (uint8_t)(p->t + 1);
  for (int i = 0; i < 4; ++i) {
    const int x = c[i].x;
    const int y = c[i].y;
    if (board_in_bounds(x, y)) b->cells[y][x] = v;
  }
}

static bool row_full(const Board* b, int y) {
  for (int x = 0; x < BOARD_W; ++x) {
    if (b->cells[y][x] == 0u) return false;
  }
  return true;
}

int board_clear_lines(Board* b, int cleared_rows_out[4]) {
  if (!b) return 0;
  int cleared = 0;
  for (int y = 0; y < BOARD_H; ++y) {
    if (row_full(b, y)) {
      if (cleared < 4 && cleared_rows_out) cleared_rows_out[cleared] = y;
      cleared++;
      // move tudo acima para baixo
      for (int yy = y; yy > 0; --yy) {
        memcpy(b->cells[yy], b->cells[yy - 1], BOARD_W);
      }
      memset(b->cells[0], 0, BOARD_W);
    }
  }
  return cleared;
}

int board_drop_distance(const Board* b, const Piece* p) {
  if (!b || !p) return 0;
  Piece tmp = *p;
  int dist = 0;
  while (true) {
    tmp.y += 1;
    if (!board_can_place(b, &tmp)) break;
    dist++;
  }
  return dist;
}