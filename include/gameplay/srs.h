#pragma once
#include <stdbool.h>
#include "gameplay/board.h"

typedef struct IRotationSystem IRotationSystem;

typedef bool (*RotateFn)(const IRotationSystem* rs, const Board* b, const Piece* in, int dir, Piece* out);

struct IRotationSystem {
  RotateFn rotate;
};

const IRotationSystem* srs_system(void);