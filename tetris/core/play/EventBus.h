#pragma once
#include <functional>
#include <vector>
#include "play/IGameEventListener.h"

// 이벤트를 구독자 모두에게 전달한다. 구독자를 소유하지 않는다 (reference_wrapper).
class EventBus {
public:
    void subscribe(IGameEventListener& listener);
    void publish(const GameEvent& event);
    void publish(GameEvent::Type type);
    void publish(GameEvent::Type type, int value);

private:
    std::vector<std::reference_wrapper<IGameEventListener>> m_listeners;
};
