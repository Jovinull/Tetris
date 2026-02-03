#include "core/state.h"
#include "utils/log.h"
#include <stddef.h>

void state_stack_init(StateStack* ss) {
  if (!ss) return;
  ss->top = -1;
}

static void call_enter(State* s, struct App* app) {
  if (s && s->v && s->v->on_enter) s->v->on_enter(s, app);
}
static void call_exit(State* s, struct App* app) {
  if (s && s->v && s->v->on_exit) s->v->on_exit(s, app);
}
static void call_destroy(State* s) {
  if (s && s->v && s->v->destroy) s->v->destroy(s);
}

bool state_stack_push(StateStack* ss, struct App* app, State* s) {
  if (!ss || !s) return false;
  if (ss->top >= 15) return false;
  ss->stack[++ss->top] = s;
  log_info("State push: %s", s->name ? s->name : "(unnamed)");
  call_enter(s, app);
  return true;
}

bool state_stack_pop(StateStack* ss, struct App* app) {
  if (!ss || ss->top < 0) return false;
  State* s = ss->stack[ss->top];
  call_exit(s, app);
  call_destroy(s);
  ss->stack[ss->top] = NULL;
  ss->top--;
  log_info("State pop");
  return true;
}

State* state_stack_top(StateStack* ss) {
  if (!ss || ss->top < 0) return NULL;
  return ss->stack[ss->top];
}

bool state_stack_replace(StateStack* ss, struct App* app, State* s) {
  if (!ss || !s) return false;
  (void)state_stack_pop(ss, app);
  return state_stack_push(ss, app, s);
}