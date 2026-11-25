#include "flow/PrologueScene.h"
#include "flow/AlbumScene.h"
#include "flow/StoryArt.h"
#include "flow/TitleScene.h"

PrologueScene::PrologueScene(GameServices& services, int stageIndex) : Scene(services), m_stageIndex(stageIndex) {}

void PrologueScene::onEnter() {
    m_services.story.house().start(m_stageIndex);
}

void PrologueScene::handle(Command command) {
    if (command == Command::Quit) {
        changeTo(std::make_unique<TitleScene>(m_services));
        return;
    }
    m_services.story.house().handle(command);
}

void PrologueScene::update(int dtMs) {
    PrologueView& house = m_services.story.house();
    house.update(dtMs);
    if (house.finished()) {
        changeTo(std::make_unique<AlbumScene>(m_services, m_stageIndex, AlbumScene::kNoRoundCleared));
    }
}

void PrologueScene::render(Console& console) const {
    m_services.story.house().draw(console);
}
