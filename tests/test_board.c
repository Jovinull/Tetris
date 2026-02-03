#include "gameplay/board.h"

#define ASSERT(x) do { if (!(x)) return 1; } while(0)

/* protótipo para evitar -Wmissing-prototypes */
int test_board(void);

int test_board(void) {
  Board b; board_clear(&b);

  Piece p; piece_spawn(&p, TET_T);
  p.x = 4; p.y = 2;
  ASSERT(board_can_place(&b, &p));

  board_place(&b, &p);
  ASSERT(!board_can_place(&b, &p));

  // preencher uma linha inteira e limpar
  for (int x = 0; x < BOARD_W; ++x) b.cells[BOARD_H - 1][x] = 1;
  int rows[4] = {0,0,0,0};
  const int c = board_clear_lines(&b, rows);
  ASSERT(c == 1);

  return 0;
}
