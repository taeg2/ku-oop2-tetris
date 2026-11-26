#include "ui/AnsiTerminal.h"
#include <iostream>
#include "common/TextUtil.h"
using namespace std;

void AnsiTerminal::open(int, int) {
    m_buffer += "\x1b[?1049h";  // 대체 화면 사용 (끝나면 원래 화면 복구)
    m_buffer += "\x1b[?25l";    // 커서 숨기기
    clearScreen();
    flush();
}

void AnsiTerminal::close() {
    m_buffer += "\x1b[0m\x1b[?25h\x1b[?1049l";
    flush();
}

void AnsiTerminal::clearScreen() {
    m_buffer += "\x1b[0m\x1b[2J\x1b[H";
}

int AnsiTerminal::ansiIndex(Color color) {
    // Windows 색 번호(파랑=1, 초록=2, 빨강=4)를 ANSI 순서(빨강=1, 초록=2, 파랑=4)로 바꾼다.
    const int c = static_cast<int>(color) & 7;
    return ((c & 1) << 2) | (c & 2) | ((c & 4) >> 2);
}

void AnsiTerminal::write(Point pos, const u32string& text, Color fg, Color bg) {
    const bool brightFg = (static_cast<int>(fg) & 8) != 0;
    const bool brightBg = (static_cast<int>(bg) & 8) != 0;
    m_buffer += "\x1b[" + to_string(pos.y + 1) + ";" + to_string(pos.x + 1) + "H";
    m_buffer += "\x1b[" + to_string((brightFg ? 90 : 30) + ansiIndex(fg)) + ";" +
                to_string((brightBg ? 100 : 40) + ansiIndex(bg)) + "m";
    m_buffer += TextUtil::encode(text);
}

void AnsiTerminal::writeRaw(Point pos, const string& bytes) {
    m_buffer += "\x1b[" + to_string(pos.y + 1) + ";" + to_string(pos.x + 1) + "H";
    m_buffer += bytes;
}

void AnsiTerminal::flush() {
    cout << m_buffer;
    cout.flush();
    m_buffer.clear();
}
