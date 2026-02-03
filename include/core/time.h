#pragma once
#include <stdint.h>

typedef struct TimeStep {
  double dt_fixed;
  double accumulator;
  double max_accumulator;
  uint64_t last_counter;
  double freq;
} TimeStep;

void time_init(TimeStep* t, double dt_fixed);
double time_begin_frame(TimeStep* t);
int time_should_update(TimeStep* t);
void time_consume_tick(TimeStep* t);