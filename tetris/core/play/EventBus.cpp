#include "play/EventBus.h"
#include <algorithm>
using namespace std;

void EventBus::subscribe(IGameEventListener& listener) {
    m_listeners.push_back(&listener);
}

void EventBus::publish(const GameEvent& event) {
    for_each(m_listeners.begin(), m_listeners.end(),
                  [&event](IGameEventListener* listener) { listener->onEvent(event); });
}

void EventBus::publish(GameEvent::Type type) {
    publish(GameEvent{type});
}

void EventBus::publish(GameEvent::Type type, int value) {
    publish(GameEvent{type, value});
}
