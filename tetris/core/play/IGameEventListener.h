#pragma once
#include "play/GameEvent.h"

// 이벤트 구독자 인터페이스 (옵저버 패턴).
//     사진 진행이 "누가 반응하는지" 몰라도 되게 해 준다.
class IGameEventListener {
public:
    virtual ~IGameEventListener() = default;
    virtual void onEvent(const GameEvent& event) = 0;

protected:
    IGameEventListener() = default;
    IGameEventListener(const IGameEventListener&) = default;
    IGameEventListener(IGameEventListener&&) = default;
    IGameEventListener& operator=(const IGameEventListener&) = default;
    IGameEventListener& operator=(IGameEventListener&&) = default;
};
