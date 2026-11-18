#include "flow/NarrativeScene.h"
#include "common/GameConfig.h"
#include "ui/ISoundPlayer.h"

namespace {
constexpr Point kTextBoxOrigin{10, 21};
constexpr int kTextBoxWidth = 80;
constexpr int kTextBoxHeight = 6;
}  // namespace

NarrativeScene::NarrativeScene(GameServices& services, std::vector<NarrativeLine> lines)
    : Scene(services), m_textBox(kTextBoxOrigin, kTextBoxWidth, kTextBoxHeight), m_lines(std::move(lines)) {
    m_textBox.setFramed(true, Color::DarkGray);
    m_textBox.setTypingSpeed(config::kTypingCharsPerSecond);
}

void NarrativeScene::onEnter() {
    if (!m_lines.empty()) {
        showLine(0);
    }
}

void NarrativeScene::showLine(std::size_t index) {
    m_index = index;
    m_lineMs = 0;
    m_textBox.setText(m_lines[index].speaker, m_lines[index].text);
    onLineShown(index);
}

void NarrativeScene::handle(Command command) {
    if (m_finished) {
        return;
    }
    if (command == Command::Quit) {
        quit();
        return;
    }
    if (command == Command::Pause) {  // Esc: 건너뛰기
        m_finished = true;
        onNarrationEnd();
        return;
    }
    if (command != Command::Confirm && command != Command::HardDrop) {
        return;
    }
    if (!m_textBox.isFullyShown()) {
        m_textBox.showAll();
        return;
    }
    if (!canAdvance()) {
        return;
    }
    m_services.sound.play(SoundId::Select);
    if (m_index + 1 < m_lines.size()) {
        showLine(m_index + 1);
    } else {
        m_finished = true;
        onNarrationEnd();
    }
}

void NarrativeScene::update(int dtMs) {
    m_lineMs += dtMs;
    m_textBox.update(dtMs);
    onUpdate(dtMs);
}

void NarrativeScene::render(Console& console) const {
    renderBackground(console);
    m_textBox.draw(console);
    drawHelp(console, "Enter/Space: 다음    Esc: 건너뛰기");
}

void NarrativeScene::onLineShown(std::size_t) {}
void NarrativeScene::onUpdate(int) {}
bool NarrativeScene::canAdvance() const {
    return true;
}
