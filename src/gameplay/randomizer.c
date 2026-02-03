#include "gameplay/randomizer.h"
#include <stdlib.h>
#include <string.h>

typedef struct Rng {
  uint32_t s;
} Rng;

static uint32_t xorshift32(Rng* r) {
  uint32_t x = r->s ? r->s : 0x12345678u;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  r->s = x;
  return x;
}

static int rnd_range(Rng* r, int n) {
  if (n <= 0) return 0;
  return (int)(xorshift32(r) % (uint32_t)n);
}

typedef struct Rand7Bag {
  IRandomizer api;
  Rng rng;
  Tetromino bag[7];
  int idx;
} Rand7Bag;

static void bag_refill(Rand7Bag* r) {
  r->bag[0]=TET_I; r->bag[1]=TET_O; r->bag[2]=TET_T; r->bag[3]=TET_S;
  r->bag[4]=TET_Z; r->bag[5]=TET_J; r->bag[6]=TET_L;
  // Fisher-Yates
  for (int i = 6; i > 0; --i) {
    const int j = rnd_range(&r->rng, i + 1);
    const Tetromino tmp = r->bag[i];
    r->bag[i] = r->bag[j];
    r->bag[j] = tmp;
  }
  r->idx = 0;
}

static Tetromino next_7bag(IRandomizer* rr) {
  Rand7Bag* r = (Rand7Bag*)rr;
  if (r->idx >= 7) bag_refill(r);
  return r->bag[r->idx++];
}

static void reset_7bag(IRandomizer* rr, uint32_t seed) {
  Rand7Bag* r = (Rand7Bag*)rr;
  r->rng.s = seed ? seed : 0xC0FFEEu;
  bag_refill(r);
}

static void destroy_7bag(IRandomizer* rr) {
  free(rr);
}

IRandomizer* randomizer_7bag_create(void) {
  Rand7Bag* r = (Rand7Bag*)calloc(1u, sizeof(*r));
  if (!r) return NULL;
  r->api.next = next_7bag;
  r->api.reset = reset_7bag;
  r->api.destroy = destroy_7bag;
  reset_7bag((IRandomizer*)r, 0xC0FFEEu);
  return (IRandomizer*)r;
}

typedef struct RandPure {
  IRandomizer api;
  Rng rng;
} RandPure;

static Tetromino next_pure(IRandomizer* rr) {
  RandPure* r = (RandPure*)rr;
  const int v = rnd_range(&r->rng, 7);
  return (Tetromino)v;
}
static void reset_pure(IRandomizer* rr, uint32_t seed) {
  RandPure* r = (RandPure*)rr;
  r->rng.s = seed ? seed : 0xBADC0DEu;
}
static void destroy_pure(IRandomizer* rr) { free(rr); }

IRandomizer* randomizer_pure_create(void) {
  RandPure* r = (RandPure*)calloc(1u, sizeof(*r));
  if (!r) return NULL;
  r->api.next = next_pure;
  r->api.reset = reset_pure;
  r->api.destroy = destroy_pure;
  reset_pure((IRandomizer*)r, 0xBADC0DEu);
  return (IRandomizer*)r;
}