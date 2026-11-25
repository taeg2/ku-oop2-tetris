#include "flow/TitleScene.h"
#include <random>
#include "stage/Album.h"
#include "ui/ISoundPlayer.h"
#include "flow/PrologueScene.h"
#include "flow/StoryArt.h"

namespace {
const int kShuffleMs = 1200;
const int kLetterRows = 5;
// T E T R I S 를 3x5 도트로 표현 ('#' = 칸)
const std::array<std::array<const char*, kLetterRows>, 6> kLogo = {{
    {{"###", ".#.", ".#.", ".#.", ".#."}},
    {{"###", "#..", "##.", "#..", "###"}},
    {{"###", ".#.", ".#.", ".#.", ".#."}},
    {{"##.", "#.#", "##.", "#.#", "#.#"}},
    {{"###", ".#.", ".#.", ".#.", "###"}},
    {{".##", "#..", ".#.", "..#", "##."}},
}};
const std::array<Color, 6> kLogoColors = {Color::Red, Color::Yellow, Color::Green, Color::Cyan, Color::Blue,
                                          Color::Magenta};
}  // namespace

TitleScene::TitleScene(GameServices& services) : Scene(services) {
    shuffleDecorations();
}

void TitleScene::onEnter() {
    m_services.album.clear();  // 타이틀로 돌아오면 새 게임
    m_services.story.restart();
}

void TitleScene::shuffleDecorations() {
    std::uniform_int_distribution<int> type(0, kTetrominoTypeCount - 1);
    std::uniform_int_distribution<int> turns(0, 3);
    for (Tetromino& piece : m_decorations) {
        piece = Tetromino(static_cast<TetrominoType>(type(m_services.rng))).rotated(turns(m_services.rng));
    }
}

void TitleScene::handle(Command command) {
    if (command == Command::Quit) {
        quit();
        return;
    }
    if (command == Command::Confirm || command == Command::HardDrop) {
        m_services.sound.play(SoundId::Select);
        changeTo(std::make_unique<PrologueScene>(m_services, 0));
    }
}

void TitleScene::update(int dtMs) {
    m_timeMs += dtMs;
    m_shuffleMs += dtMs;
    if (m_shuffleMs >= kShuffleMs) {
        m_shuffleMs = 0;
        shuffleDecorations();
    }
}

void TitleScene::render(Console& console) const {
    const int logoLeft = (console.width() - 6 * 4 * 2) / 2;
    for (std::size_t letter = 0; letter < kLogo.size(); ++letter) {
        for (int row = 0; row < kLetterRows; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (kLogo[letter][row][col] == '#') {
                    const int x = logoLeft + static_cast<int>(letter) * 8 + col * 2;
                    console.fill({x, 4 + row, 2, 1}, kLogoColors[letter]);
                }
            }
        }
    }
    console.drawCentered(11, "기억을 맞추는 테트리스", Color::White);
    console.drawCentered(13, "- 아버지의 앨범을 다시 채우는 이야기 -", Color::Gray);

    const std::array<int, 4> columns = {30, 42, 54, 66};
    for (std::size_t i = 0; i < m_decorations.size(); ++i) {
        for (Point p : m_decorations[i].localCells()) {
            const Color color = m_decorations[i].color();
            console.fill({columns[i] + p.x * 2, 16 + p.y, 2, 1}, color);
        }
    }
    if ((m_timeMs / 500) % 2 == 0) {
        console.drawCentered(23, "Enter 또는 Space 를 눌러 시작", Color::Yellow);
    }
    drawHelp(console, "Q: 종료    (실행 인자 --mute: 소리 끄기)");
}
