#include "gameplay/srs.h"
#include "gameplay/board.h"

#define ASSERT(x) do { if (!(x)) return 1; } while(0)

int test_srs(void) {
  Board b; board_clear(&b);

  Piece p; piece_spawn(&p, TET_I);
  p.x = 0; p.y = 3; // encostado na parede
  const IRotationSystem* rs = srs_system();

  Piece out;
  const bool ok = rs->rotate(rs, &b, &p, +1, &out);
  ASSERT(ok); // deve kickar

  return 0;
}