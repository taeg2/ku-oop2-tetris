// Console.h - everything Windows-specific: Windows Terminal, VT mode, writing, window size, keyboard.
#pragma once
#include <string>

enum class Key { None, Left, Right, Rotate, SoftDrop, HardDrop, Enter, Escape, Pause, Round1, Event, Jump1, Jump2, Jump3, Jump4 };   // Round1 = restart this round, JumpN = go to round N

class Console {
public:
    // reopen the game inside Windows Terminal (Sixel needs WT 1.22+); true if we relaunched
    static bool relaunchInWindowsTerminal();
    static void init();                          // VT sequences on, cursor hidden, screen cleared
    static void shutdown();
    static void write(const std::string& s);
    static void size(int& cols, int& rows);
    static void setTitle(const std::string& s);
};

class InputHandler {
public:
    Key poll();                                  // one key per call, Key::None when nothing is waiting
};