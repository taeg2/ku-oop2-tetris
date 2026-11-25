#ifndef _WIN32
#include "ui/PosixKeyboardInput.h"
#include <string>
#include <termios.h>
#include <unistd.h>

struct PosixKeyboardInput::SavedMode {
    termios original{};
};

namespace {
const char kEscape = 27;
}

PosixKeyboardInput::PosixKeyboardInput() : m_saved(std::make_unique<SavedMode>()) {
    tcgetattr(STDIN_FILENO, &m_saved->original);
    termios raw = m_saved->original;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 0;   // 기다리지 않는다
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

PosixKeyboardInput::~PosixKeyboardInput() {
    tcsetattr(STDIN_FILENO, TCSANOW, &m_saved->original);
}

void PosixKeyboardInput::update() {
    std::string bytes;
    char buffer[64];
    ssize_t count = 0;
    while ((count = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        bytes.append(buffer, static_cast<std::size_t>(count));
    }
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        // 방향키는 ESC [ A~D 세 글자로 들어온다.
        if (bytes[i] == kEscape && i + 2 < bytes.size() && bytes[i + 1] == '[') {
            switch (bytes[i + 2]) {
                case 'A': pushArrow(Arrow::Up); break;
                case 'B': pushArrow(Arrow::Down); break;
                case 'C': pushArrow(Arrow::Right); break;
                case 'D': pushArrow(Arrow::Left); break;
                default: break;
            }
            i += 2;
        } else if (bytes[i] == kEscape) {
            pushEscape();
        } else {
            pushKey(static_cast<unsigned char>(bytes[i]));
        }
    }
}
#endif  // _WIN32
