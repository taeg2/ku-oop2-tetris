#include "flow/GameOverScene.h"
#include "common/GameConfig.h"
#include "flow/PlayScene.h"
#include "flow/PrologueScene.h"

GameOverScene::GameOverScene(GameServices& services, int stageIndex)
    : Scene(services), m_stageIndex(stageIndex), m_view({layout::kArtX, layout::kArtY}) {}

void GameOverScene::onEnter() {
    m_view.open();
}

void GameOverScene::handle(Command command) {
    if (command == Command::Quit) {
        quit();
        return;
    }

    m_view.handle(command);
    switch (m_view.takeChoice()) {
        case GameOverView::Choice::Retry:
            changeTo(make_unique<PlayScene>(m_services, m_stageIndex));
            break;
        case GameOverView::Choice::Home:
            changeTo(make_unique<PrologueScene>(m_services, 0));
            break;
        case GameOverView::Choice::None:
            break;
    }
}

void GameOverScene::update(int dtMs) {
    m_view.update(dtMs);
}

void GameOverScene::render(Console& console) const {
    m_view.draw(console);
}
