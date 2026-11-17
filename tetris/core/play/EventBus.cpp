#include "play/EventBus.h"

void EventBus::subscribe(IGameEventListener& listener) {
    m_listeners.emplace_back(listener);
}

void EventBus::publish(const GameEvent& event) {
    for (IGameEventListener& listener : m_listeners) {
        listener.onEvent(event);
    }
}

void EventBus::publish(GameEvent::Type type) {
    publish(GameEvent{type});
}

void EventBus::publish(GameEvent::Type type, int value) {
    publish(GameEvent{type, value});
}
