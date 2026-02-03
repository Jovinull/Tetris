#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>

struct App;

typedef struct State State;

typedef struct StateVTable {
  void (*on_enter)(State* s, struct App* app);
  void (*on_exit)(State* s, struct App* app);
  void (*handle_event)(State* s, struct App* app, const SDL_Event* e);
  void (*update)(State* s, struct App* app, double dt);
  void (*render)(State* s, struct App* app);
  void (*destroy)(State* s);
} StateVTable;

struct State {
  const StateVTable* v;
  const char* name;
};

typedef struct StateStack {
  State* stack[16];
  int top;
} StateStack;

void state_stack_init(StateStack* ss);
bool state_stack_push(StateStack* ss, struct App* app, State* s);
bool state_stack_pop(StateStack* ss, struct App* app);
State* state_stack_top(StateStack* ss);
bool state_stack_replace(StateStack* ss, struct App* app, State* s);