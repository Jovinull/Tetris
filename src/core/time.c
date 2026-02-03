#include "core/time.h"
#include <SDL2/SDL.h>

static uint64_t counter_now(void) {
  return (uint64_t)SDL_GetPerformanceCounter();
}

void time_init(TimeStep* t, double dt_fixed) {
  if (!t) return;
  t->dt_fixed = dt_fixed;
  t->accumulator = 0.0;
  t->max_accumulator = 0.25; // evita spiral of death
  t->freq = (double)SDL_GetPerformanceFrequency();
  t->last_counter = counter_now();
}

double time_begin_frame(TimeStep* t) {
  if (!t) return 0.0;
  const uint64_t now = counter_now();
  const double delta = (double)(now - t->last_counter) / t->freq;
  t->last_counter = now;
  const double clamped = (delta > 0.1) ? 0.1 : delta; // clamp de frame
  t->accumulator += clamped;
  if (t->accumulator > t->max_accumulator) t->accumulator = t->max_accumulator;
  return clamped;
}

int time_should_update(TimeStep* t) {
  if (!t) return 0;
  return t->accumulator >= t->dt_fixed;
}

void time_consume_tick(TimeStep* t) {
  if (!t) return;
  t->accumulator -= t->dt_fixed;
  if (t->accumulator < 0.0) t->accumulator = 0.0;
}