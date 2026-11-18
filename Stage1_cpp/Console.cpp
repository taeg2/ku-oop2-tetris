// Console.cpp - Windows implementation.
#include "Console.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#include <conio.h>
#include <cstdlib>

bool Console::relaunchInWindowsTerminal() {
    char buf[8];
    if (GetEnvironmentVariableA("WT_SESSION", buf, sizeof(buf)) > 0) return false;   // already inside Windows Terminal
    char exe[MAX_PATH];
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string args = std::string("-M \"") + exe + "\"";
    HINSTANCE r = ShellExecuteA(nullptr, "open", "wt.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
    return (INT_PTR)r > 32;
}

void Console::init() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(out, &mode);
    SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    write("\x1b[?25l\x1b[2J");
}

void Console::shutdown() { write("\x1b[?25h\x1b[2J\x1b[H"); }

void Console::write(const std::string& s) {
    DWORD n;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), s.data(), (DWORD)s.size(), &n, nullptr);
}

void Console::size(int& cols, int& rows) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    cols = 120; rows = 30;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        cols = info.srWindow.Right - info.srWindow.Left + 1;
        rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
}

void Console::setTitle(const std::string& s) { SetConsoleTitleA(s.c_str()); }

Key InputHandler::poll() {
    if (!_kbhit()) return Key::None;
    int k = _getch();
    if (k == 0 || k == 224) {
        switch (_getch()) {
        case 75: return Key::Left;
        case 77: return Key::Right;
        case 72: return Key::Rotate;
        case 80: return Key::SoftDrop;
        case 59: return Key::Jump1;
        case 60: return Key::Jump2;
        case 61: return Key::Jump3;
        case 62: return Key::Jump4;
        default: return Key::None;
        }
    }
    switch (k) {
    case 'a': case 'A': return Key::Left;
    case 'd': case 'D': return Key::Right;
    case 'w': case 'W': return Key::Rotate;
    case 's': case 'S': return Key::SoftDrop;
    case ' ': return Key::HardDrop;
    case 13: return Key::Enter;
    case 27: return Key::Pause;
    case 'q': case 'Q': return Key::Escape;
    case 'r': case 'R': return Key::Round1;
    case 'e': case 'E': return Key::Event;
    default: return Key::None;
    }
}

#else   // non-Windows stubs so the drawing code can be tested elsewhere
#include <cstdio>
bool Console::relaunchInWindowsTerminal() { return false; }
void Console::init() {}
void Console::shutdown() {}
void Console::write(const std::string& s) { std::fwrite(s.data(), 1, s.size(), stdout); }
void Console::size(int& cols, int& rows) { cols = 148; rows = 46; }
void Console::setTitle(const std::string&) {}
Key InputHandler::poll() { return Key::None; }
#endif