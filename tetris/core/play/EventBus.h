#pragma once
#include <vector>
#include "play/IGameEventListener.h"
using namespace std;

// 이벤트를 구독자 모두에게 전달한다. (옵저버 패턴)
//     구독자를 소유하지 않으므로 관찰용 포인터로만 들고 있다. 구독자는 버스보다 오래 살아 있어야 한다.
class EventBus {
public:
    void subscribe(IGameEventListener& listener);
    void publish(const GameEvent& event);
    void publish(GameEvent::Type type);
    void publish(GameEvent::Type type, int value);

private:
    vector<IGameEventListener*> m_listeners;
};
