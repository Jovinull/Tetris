#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "gameplay/piece.h"

enum { BOARD_W = 10, BOARD_H_VISIBLE = 20, BOARD_H_HIDDEN = 2, BOARD_H = 22 };

typedef struct Board {
  // 0 = vazio, 1..7 = tipo+1
  uint8_t cells[BOARD_H][BOARD_W];
} Board;

void board_clear(Board* b);

bool board_in_bounds(int x, int y);
bool board_cell_filled(const Board* b, int x, int y);

bool board_can_place(const Board* b, const Piece* p);
void board_place(Board* b, const Piece* p);
int board_clear_lines(Board* b, int cleared_rows_out[4]);

int board_drop_distance(const Board* b, const Piece* p);