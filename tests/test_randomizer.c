#include <stddef.h>
#include "gameplay/randomizer.h"
#include <stdbool.h>

#define ASSERT(x) do { if (!(x)) return 1; } while(0)

static bool has_all_7(const Tetromino arr[7]) {
  bool seen[7] = {0,0,0,0,0,0,0};
  for (int i = 0; i < 7; ++i) {
    const int v = (int)arr[i];
    if (v < 0 || v > 6) return false;
    if (seen[v]) return false;
    seen[v] = true;
  }
  return true;
}

int test_randomizer(void) {
  IRandomizer* r = randomizer_7bag_create();
  ASSERT(r);

  Tetromino bag[7];
  for (int i = 0; i < 7; ++i) bag[i] = r->next(r);
  ASSERT(has_all_7(bag));

  r->destroy(r);
  return 0;
}