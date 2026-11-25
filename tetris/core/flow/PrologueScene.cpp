#include "flow/PrologueScene.h"
#include "flow/AlbumScene.h"
#include "flow/StoryArt.h"
#include "stage/Album.h"
using namespace std;

PrologueScene::PrologueScene(GameServices& services, int stageIndex) : Scene(services), m_stageIndex(stageIndex) {}

void PrologueScene::onEnter() {
    if (m_stageIndex == 0) {
        // 오프닝은 새 게임의 시작이다 (타이틀 화면 대신). 앨범과 모든 그림을 처음 상태로 돌린다.
        m_services.album.clear();
        m_services.story.restart();
    }
    m_services.story.house().start(m_stageIndex);
}

void PrologueScene::handle(Command command) {
    if (command == Command::Quit) {
        quit();
        return;
    }
    m_services.story.house().handle(command);
}

void PrologueScene::update(int dtMs) {
    PrologueView& house = m_services.story.house();
    house.update(dtMs);
    if (house.finished()) {
        changeTo(make_unique<AlbumScene>(m_services, m_stageIndex, AlbumScene::kNoRoundCleared));
    }
}

void PrologueScene::render(Console& console) const {
    m_services.story.house().draw(console);
}
