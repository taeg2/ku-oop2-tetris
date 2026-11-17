#include "ui/KeyboardInput.h"
#include <stdexcept>

namespace {
constexpr int kEnter = 13;
constexpr int kNewLine = 10;
constexpr int kSpace = 32;
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
    if (const auto command = translate(key)) {
        m_commands.push_back(*command);
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

std::optional<Command> KeyboardInput::translate(int key) {
    switch (key) {
        case kSpace: return Command::HardDrop;
        case kEnter: case kNewLine: return Command::Confirm;
        case '1': return Command::UseBluePill;
        case '2': return Command::UseRedPill;
        case 'q': case 'Q': return Command::Quit;
        case 'a': case 'A': return Command::MoveLeft;
        case 'd': case 'D': return Command::MoveRight;
        case 'w': case 'W': return Command::Rotate;
        case 's': case 'S': return Command::SoftDrop;
        default: return std::nullopt;
    }
}
