#pragma once
#include "ui/Command.h"

// 입력 장치 인터페이스. 게임은 이 인터페이스에만 의존하므로 Windows 키보드, Mac 키보드,
// 테스트용 입력을 바꿔 끼울 수 있다. 쿼리(hasCommand, peekCommand)와 커맨드(update, popCommand)를 나눴다.
class IInputSource {
public:
    virtual ~IInputSource() = default;

    virtual void update() = 0;                // 들어온 키를 읽어 명령 대기열에 쌓는다
    virtual bool hasCommand() const = 0;
    virtual Command peekCommand() const = 0;  // 대기열 맨 앞 명령 (hasCommand()가 true일 때만)
    virtual void popCommand() = 0;
};
