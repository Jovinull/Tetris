#pragma once
#include <stddef.h>
#include <stdint.h>

typedef enum GameEventType {
  EV_PIECE_SPAWN = 1,
  EV_PIECE_MOVE,
  EV_PIECE_ROTATE,
  EV_PIECE_LOCK,
  EV_LINE_CLEAR,
  EV_LEVEL_UP,
  EV_HOLD,
  EV_GAME_OVER,
  EV_PAUSE_TOGGLE
} GameEventType;

typedef struct GameEvent {
  GameEventType type;
  int a;
  int b;
  int c;
} GameEvent;

typedef void (*EventListenerFn)(void* user, const GameEvent* ev);

typedef struct EventListener {
  void* user;
  EventListenerFn fn;
} EventListener;

typedef struct EventBus {
  EventListener listeners[32];
  size_t count;
} EventBus;

void event_bus_init(EventBus* bus);
int event_bus_subscribe(EventBus* bus, void* user, EventListenerFn fn);
void event_bus_publish(EventBus* bus, const GameEvent* ev);