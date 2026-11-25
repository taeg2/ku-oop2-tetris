#ifdef _WIN32
#include "ui/WindowsKeyboardInput.h"
#include <conio.h>

namespace {
const int kExtendedPrefixA = 0x00;
const int kExtendedPrefixB = 0xE0;
const int kEscape = 27;
const int kArrowUp = 72;
const int kArrowDown = 80;
const int kArrowLeft = 75;
const int kArrowRight = 77;
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
