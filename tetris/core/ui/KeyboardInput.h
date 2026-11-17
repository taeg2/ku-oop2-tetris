#pragma once
#include <deque>
#include <optional>
#include "ui/IInputSource.h"

// 키보드 입력의 공통 부분: 명령 대기열과 "키 -> 명령" 변환표.
// 운영체제마다 다른 "키를 읽는 방법"(update)만 자식 클래스가 구현한다.
class KeyboardInput : public IInputSource {
public:
    bool hasCommand() const override;
    Command peekCommand() const override;
    void popCommand() override;

protected:
    enum class Arrow { Up, Down, Left, Right };

    void pushKey(int key);       // 일반 문자 키
    void pushArrow(Arrow arrow);  // 방향키
    void pushEscape();

private:
    static std::optional<Command> translate(int key);

    std::deque<Command> m_commands;
};
