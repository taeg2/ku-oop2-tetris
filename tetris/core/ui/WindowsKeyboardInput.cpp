#ifdef _WIN32
#include "ui/WindowsKeyboardInput.h"
#include <conio.h>

namespace {
constexpr int kExtendedPrefixA = 0x00;
constexpr int kExtendedPrefixB = 0xE0;
constexpr int kEscape = 27;
constexpr int kArrowUp = 72;
constexpr int kArrowDown = 80;
constexpr int kArrowLeft = 75;
constexpr int kArrowRight = 77;
}  // namespace

void WindowsKeyboardInput::update() {
    while (_kbhit()) {
        const int key = _getch();
        if (key == kExtendedPrefixA || key == kExtendedPrefixB) {
            switch (_getch()) {
                case kArrowUp: pushArrow(Arrow::Up); break;
                case kArrowDown: pushArrow(Arrow::Down); break;
                case kArrowLeft: pushArrow(Arrow::Left); break;
                case kArrowRight: pushArrow(Arrow::Right); break;
                default: break;
            }
        } else if (key == kEscape) {
            pushEscape();
        } else {
            pushKey(key);
        }
    }
}
#endif  // _WIN32
