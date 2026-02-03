#pragma once
#include <stdint.h>
#include "gameplay/piece.h"

typedef struct IRandomizer IRandomizer;

typedef Tetromino (*NextFn)(IRandomizer* r);
typedef void (*ResetFn)(IRandomizer* r, uint32_t seed);
typedef void (*DestroyFn)(IRandomizer* r);

struct IRandomizer {
  NextFn next;
  ResetFn reset;
  DestroyFn destroy;
};

IRandomizer* randomizer_7bag_create(void);
IRandomizer* randomizer_pure_create(void);