#include "flow/AlbumScene.h"
#include "common/GameConfig.h"
#include "flow/EndingScene.h"
#include "flow/PlayScene.h"
#include "flow/PrologueScene.h"
#include "flow/StoryArt.h"
#include "flow/TitleScene.h"
#include "stage/StageCatalog.h"

AlbumScene::AlbumScene(GameServices& services, int stageIndex, int clearedRound)
    : Scene(services), m_stageIndex(stageIndex), m_clearedRound(clearedRound) {}

void AlbumScene::onEnter() {
    m_services.story.album().open(m_stageIndex, m_clearedRound);
    if (m_clearedRound != kNoRoundCleared) {
        m_services.sound.play(SoundId::StageClear);
    }
}

bool AlbumScene::isStageDone() const {
    return m_clearedRound == StoryArt::kRoundsPerStage - 1;
}

bool AlbumScene::isLastStage() const {
    return m_stageIndex + 1 >= m_services.catalog.count();
}

void AlbumScene::handle(Command command) {
    if (command == Command::Quit) {
        changeTo(std::make_unique<TitleScene>(m_services));
        return;
    }
    if (isStageDone()) {
        // 스테이지의 네 칸을 다 채웠다. 앨범이 스스로 페이지를 넘길 때까지 입력을 받지 않고,
        //     마지막 스테이지면 Enter로 바로 엔딩에 간다.
        if (isLastStage() && (command == Command::Confirm || command == Command::HardDrop)) {
            changeTo(std::make_unique<EndingScene>(m_services));
        }
        return;
    }
    m_services.story.album().handle(command);
}

void AlbumScene::update(int dtMs) {
    m_timeMs += dtMs;
    AlbumView& album = m_services.story.album();
    album.update(dtMs);

    if (album.hasPick()) {
        album.takePick();  // 라운드는 순서대로 이어서 하므로 고른 칸 번호는 쓰지 않는다
        m_services.sound.play(SoundId::Select);
        changeTo(std::make_unique<PlayScene>(m_services, m_stageIndex));
        return;
    }
    if (album.stageFinished()) {
        changeTo(std::make_unique<PrologueScene>(m_services, m_stageIndex + 1));
    } else if (isStageDone() && isLastStage() && m_timeMs >= config::kFinalAlbumMs) {
        changeTo(std::make_unique<EndingScene>(m_services));
    }
}

void AlbumScene::render(Console& console) const {
    m_services.story.album().draw(console);
}
