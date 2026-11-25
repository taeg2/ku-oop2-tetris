#include "flow/EndingScene.h"
#include "flow/StoryArt.h"
#include "flow/TitleScene.h"

EndingScene::EndingScene(GameServices& services) : Scene(services) {}

void EndingScene::onEnter() {
    m_services.story.ending().start();
}

void EndingScene::handle(Command command) {
    if (command == Command::Quit) {
        quit();
        return;
    }
    // EndingView::handle()의 건너뛰기는 시작 직후에 누르면 시작 시각이 음수가 되어 엔딩이 멈춘다.
    //     ui 쪽에서 고칠 때까지 엔딩 중에는 입력을 넘기지 않는다.
    if (!m_services.story.ending().finished()) {
        return;
    }
    if (command == Command::Confirm || command == Command::HardDrop || command == Command::Pause) {
        m_services.sound.play(SoundId::Select);
        changeTo(std::make_unique<TitleScene>(m_services));
    }
}

void EndingScene::update(int dtMs) {
    m_services.story.ending().update(dtMs);
}

void EndingScene::render(Console& console) const {
    m_services.story.ending().draw(console);
}
