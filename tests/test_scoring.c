#include <stddef.h>
#include "gameplay/scoring.h"

#define ASSERT(x) do { if (!(x)) return 1; } while(0)

// protótipo anterior para evitar -Wmissing-prototypes
int test_scoring(void);

int test_scoring(void) {
  IScoring* s = scoring_modern_lite_create();
  ASSERT(s);
  s->reset(s, 1);

  // tetris
  ScoreResult r = s->on_clear(s, 4, false, 0);
  const ScoreState* st = s->get(s);
  ASSERT(r.cleared == 4);
  ASSERT(st->score > 0);

  // level up após 10 linhas
  for (int i = 0; i < 6; ++i) (void)s->on_clear(s, 1, false, 0); // +6 linhas (total 10)
  st = s->get(s);
  ASSERT(st->level >= 2);

  s->destroy(s);
  return 0;
}
