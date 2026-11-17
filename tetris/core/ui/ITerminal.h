#pragma once
#include <string>
#include "common/Color.h"
#include "common/Point.h"

// 실제 터미널 하나를 추상화한다. Console(화면 버퍼)은 이 인터페이스에만 의존하므로
// Windows 콘솔(WindowsTerminal)과 Mac/Linux 터미널(AnsiTerminal)을 바꿔 끼울 수 있다. (DIP)
class ITerminal {
public:
    virtual ~ITerminal() = default;

    virtual void open(int width, int height) = 0;  // 창 크기 맞추기, 커서 숨기기
    virtual void close() = 0;                       // 원래 상태로 되돌리기
    virtual void clearScreen() = 0;
    virtual void write(Point pos, const std::u32string& text, Color fg, Color bg) = 0;
    virtual void flush() = 0;
};
