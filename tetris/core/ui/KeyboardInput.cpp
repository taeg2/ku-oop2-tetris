#include "ui/KeyboardInput.h"
#include <stdexcept>

namespace {
const int kEnter = 13;
const int kNewLine = 10;
const int kSpace = 32;
}  // namespace

bool KeyboardInput::hasCommand() const {
    return !m_commands.empty();
}

Command KeyboardInput::peekCommand() const {
    if (m_commands.empty()) {
        throw std::logic_error("KeyboardInput::peekCommand: 대기 중인 명령이 없습니다");
    }
    return m_commands.front();
}

void KeyboardInput::popCommand() {
    if (!m_commands.empty()) {
        m_commands.pop_front();
    }
}

void KeyboardInput::pushKey(int key) {
    Command command = Command::Confirm;
    if (translate(key, command)) {
        m_commands.push_back(command);
    }
}

void KeyboardInput::pushArrow(Arrow arrow) {
    switch (arrow) {
        case Arrow::Up: m_commands.push_back(Command::Rotate); break;
        case Arrow::Down: m_commands.push_back(Command::SoftDrop); break;
        case Arrow::Left: m_commands.push_back(Command::MoveLeft); break;
        case Arrow::Right: m_commands.push_back(Command::MoveRight); break;
    }
}

void KeyboardInput::pushEscape() {
    m_commands.push_back(Command::Pause);
}

bool KeyboardInput::translate(int key, Command& command) {
    switch (key) {
        case kSpace: command = Command::HardDrop; return true;
        case kEnter: case kNewLine: command = Command::Confirm; return true;
        case '1': command = Command::UseBluePill; return true;
        case '2': command = Command::UseRedPill; return true;
        case 'q': case 'Q': command = Command::Quit; return true;
        case 'a': case 'A': command = Command::MoveLeft; return true;
        case 'd': case 'D': command = Command::MoveRight; return true;
        case 'w': case 'W': command = Command::Rotate; return true;
        case 's': case 'S': command = Command::SoftDrop; return true;
        default: return false;
    }
}
