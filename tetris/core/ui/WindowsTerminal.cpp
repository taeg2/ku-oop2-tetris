#ifdef _WIN32
// windows.h를 가장 먼저 include한다 (using namespace std; 뒤에 오면 byte 같은 이름이 겹칠 수 있다)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "ui/WindowsTerminal.h"
#include "common/ConsoleException.h"
using namespace std;

namespace {
WORD attributeOf(Color fg, Color bg) {
    return static_cast<WORD>(static_cast<int>(fg) | (static_cast<int>(bg) << 4));
}

wstring toUtf16(const u32string& text) {
    wstring out;
    for (char32_t ch : text) {
        if (ch < 0x10000) {
            out.push_back(static_cast<wchar_t>(ch));
        } else {
            ch -= 0x10000;
            out.push_back(static_cast<wchar_t>(0xD800 + (ch >> 10)));
            out.push_back(static_cast<wchar_t>(0xDC00 + (ch & 0x3FF)));
        }
    }
    return out;
}
}

void WindowsTerminal::open(int width, int height) {
    m_output = GetStdHandle(STD_OUTPUT_HANDLE);
    m_input = GetStdHandle(STD_INPUT_HANDLE);
    if (m_output == INVALID_HANDLE_VALUE || m_output == nullptr) {
        throw ConsoleException("GetStdHandle", GetLastError());
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitleW(L"기억을 맞추는 테트리스");

    // 창과 버퍼 크기를 게임 화면에 맞춘다. (Windows Terminal에서는 무시될 수 있음)
    HANDLE out = static_cast<HANDLE>(m_output);
    SMALL_RECT tiny{0, 0, 1, 1};
    SetConsoleWindowInfo(out, TRUE, &tiny);
    SetConsoleScreenBufferSize(out, COORD{static_cast<SHORT>(width), static_cast<SHORT>(height)});
    SMALL_RECT window{0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1)};
    SetConsoleWindowInfo(out, TRUE, &window);

    // 마우스로 화면을 드래그하면 출력이 멈추는 "빠른 편집 모드"를 끈다.
    DWORD mode = 0;
    if (GetConsoleMode(static_cast<HANDLE>(m_input), &mode)) {
        m_originalInputMode = mode;
        SetConsoleMode(static_cast<HANDLE>(m_input), (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_EXTENDED_FLAGS);
    }
    setCursorVisible(false);
    clearScreen();
}

void WindowsTerminal::close() {
    SetConsoleTextAttribute(static_cast<HANDLE>(m_output), attributeOf(Color::Gray, Color::Black));
    clearScreen();
    setCursorVisible(true);
    if (m_originalInputMode != 0) {
        SetConsoleMode(static_cast<HANDLE>(m_input), m_originalInputMode);
    }
}

void WindowsTerminal::clearScreen() {
    HANDLE out = static_cast<HANDLE>(m_output);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (!GetConsoleScreenBufferInfo(out, &info)) {
        return;
    }
    const DWORD cells = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
    DWORD written = 0;
    FillConsoleOutputCharacterW(out, L' ', cells, COORD{0, 0}, &written);
    FillConsoleOutputAttribute(out, attributeOf(Color::Gray, Color::Black), cells, COORD{0, 0}, &written);
    SetConsoleCursorPosition(out, COORD{0, 0});
}

void WindowsTerminal::write(Point pos, const u32string& text, Color fg, Color bg) {
    HANDLE out = static_cast<HANDLE>(m_output);
    SetConsoleCursorPosition(out, COORD{static_cast<SHORT>(pos.x), static_cast<SHORT>(pos.y)});
    SetConsoleTextAttribute(out, attributeOf(fg, bg));
    const wstring wide = toUtf16(text);
    DWORD written = 0;
    WriteConsoleW(out, wide.c_str(), static_cast<DWORD>(wide.size()), &written, nullptr);
}

void WindowsTerminal::writeRaw(Point pos, const string& bytes) {
    HANDLE out = static_cast<HANDLE>(m_output);
    DWORD mode = 0;
    if (GetConsoleMode(out, &mode) && !(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleCursorPosition(out, COORD{static_cast<SHORT>(pos.x), static_cast<SHORT>(pos.y)});
    DWORD written = 0;
    WriteFile(out, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
}

void WindowsTerminal::flush() {
    // WriteConsoleW는 바로 출력되므로 따로 할 일이 없다.
}

void WindowsTerminal::setCursorVisible(bool visible) {
    CONSOLE_CURSOR_INFO info{};
    info.dwSize = 1;
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(static_cast<HANDLE>(m_output), &info);
}
#endif  // _WIN32
