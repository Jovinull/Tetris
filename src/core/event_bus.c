#include "core/event_bus.h"

void event_bus_init(EventBus* bus) {
  if (!bus) return;
  bus->count = 0u;
}

int event_bus_subscribe(EventBus* bus, void* user, EventListenerFn fn) {
  if (!bus || !fn) return 0;
  if (bus->count >= (sizeof(bus->listeners) / sizeof(bus->listeners[0]))) return 0;
  bus->listeners[bus->count++] = (EventListener){ user, fn };
  return 1;
}

void event_bus_publish(EventBus* bus, const GameEvent* ev) {
  if (!bus || !ev) return;
  for (size_t i = 0u; i < bus->count; ++i) {
    bus->listeners[i].fn(bus->listeners[i].user, ev);
  }
}