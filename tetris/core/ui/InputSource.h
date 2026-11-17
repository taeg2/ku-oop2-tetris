#pragma once

#include <vector>

#include "ui/Action.h"

// 키 입력의 추상 클래스.
// 게임 쪽은 이 클래스로 조작(Action)만 받으므로, 입력 방식이 바뀌어도 게임 코드는 고치지 않는다. (DIP)
//
// 사용 방법: 매 프레임 poll()로 입력을 모은 뒤, getActions()로 이번 프레임의 조작을 읽는다.
class InputSource {
public:
    virtual ~InputSource();

    // ===== 쿼리 =====

    // 마지막 poll() 때 모은 조작 (누른 순서대로)
    virtual std::vector<Action> getActions() const = 0;

    // ===== 커맨드 =====

    // 지난 poll() 이후에 들어온 입력을 모은다. 입력이 없으면 기다리지 않고 바로 돌아온다.
    virtual void poll() = 0;
};
