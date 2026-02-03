#pragma once
#include <stdbool.h>

typedef struct ScoreState {
  int score;
  int level;
  int lines_total;
  int combo;
  int b2b;
} ScoreState;

typedef struct ScoreResult {
  int add_score;
  int cleared;
  bool tetris;
  bool level_up;
} ScoreResult;

typedef struct IScoring IScoring;

typedef ScoreResult (*OnClearFn)(IScoring* s, int cleared, bool hard_drop, int drop_dist);
typedef void (*ResetScoreFn)(IScoring* s, int start_level);
typedef const ScoreState* (*GetStateFn)(const IScoring* s);
typedef void (*DestroyScoreFn)(IScoring* s);

struct IScoring {
  OnClearFn on_clear;
  ResetScoreFn reset;
  GetStateFn get;
  DestroyScoreFn destroy;
};

IScoring* scoring_classic_create(void);
IScoring* scoring_modern_lite_create(void);