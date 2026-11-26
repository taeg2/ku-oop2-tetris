#pragma once
#include <string>
#include "ui/ITerminal.h"
using namespace std;

// ANSI 이스케이프 코드로 출력한다. Mac/Linux 터미널용이며 표준 C++(cout)만 쓴다.
// 한 프레임 동안 모은 출력을 flush()에서 한 번에 내보내 깜빡임을 줄인다.
class AnsiTerminal : public ITerminal {
public:
    void open(int width, int height) override;
    void close() override;
    void clearScreen() override;
    void write(Point pos, const u32string& text, Color fg, Color bg) override;
    void flush() override;
    void writeRaw(Point pos, const string& bytes) override;

private:
    static int ansiIndex(Color color);

    string m_buffer;
};
