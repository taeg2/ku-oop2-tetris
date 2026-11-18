// main.cpp - Stage 1, Round 1 "A Breezy Day at the Lake". Needs Windows Terminal 1.22+ for Sixel.
// Keys: Left/Right move, Up rotate, Down soft drop, Space hard drop, W gust of wind,
//       Enter = press the NEXT ROUND button (or retry after a top-out), R restart, Esc quit.
#include "Game.h"
#include <cstdlib>
#include <ctime>

int main() {
    if (Console::relaunchInWindowsTerminal()) return 0;   // the copy inside Windows Terminal takes over
    std::srand((unsigned)std::time(nullptr));
    Console::init();
    Game game;
    game.run();
    Console::shutdown();
    return 0;
}
