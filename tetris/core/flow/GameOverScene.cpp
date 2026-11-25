#include "flow/GameOverScene.h"
#include "stage/StageCatalog.h"
#include "ui/ISoundPlayer.h"
#include "ui/PhotoPainter.h"
#include "flow/PlayScene.h"
#include "flow/TitleScene.h"

GameOverScene::GameOverScene(GameServices& services, int stageIndex) : Scene(services), m_stageIndex(stageIndex) {}

void GameOverScene::onEnter() {
    m_services.sound.play(SoundId::GameOver);
}

void GameOverScene::handle(Command command) {
    if (command == Command::Confirm || command == Command::HardDrop) {
        m_services.sound.play(SoundId::Select);
        changeTo(std::make_unique<PlayScene>(m_services, m_stageIndex));  // 지금 라운드부터 보드를 비우고 다시
    } else if (command == Command::Pause) {
        changeTo(std::make_unique<TitleScene>(m_services));
    } else if (command == Command::Quit) {
        quit();
    }
}

void GameOverScene::update(int dtMs) {
    m_timeMs += dtMs;
}

void GameOverScene::render(Console& console) const {
    const StageConfig& stage = m_services.catalog.get(m_stageIndex);
    const Point photoOrigin{(console.width() - PhotoArt::kWidth * 2) / 2, 2};
    PhotoPainter::paint(console, photoOrigin, stage.photo, PhotoTone::Hidden);

    const Rect box{20, 11, 60, 10};
    drawDialog(console, box, Color::DarkRed);
    console.drawCentered(box.y + 2, stage.title + "  " + stage.period, Color::Gray);
    console.drawCentered(box.y + 4, "기억이 다시 흐려졌습니다.", Color::Red);
    console.drawCentered(box.y + 5, "이 시기의 기억을 다시 찾아볼까요?", Color::White);
    if ((m_timeMs / 500) % 2 == 0) {
        console.drawCentered(box.y + 7, "Enter: 다시 찾기", Color::Yellow);
    }
    drawHelp(console, "Esc: 타이틀로    Q: 종료");
}
