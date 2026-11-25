#include "flow/PlayScene.h"
#include "common/GameConfig.h"
#include "flow/AlbumScene.h"
#include "flow/PrologueScene.h"
#include "flow/StoryArt.h"
#include "stage/Album.h"
using namespace std;

PlayScene::PlayScene(GameServices& services, int stageIndex) : Scene(services), m_stageIndex(stageIndex) {
    m_services.story.beginStage(stageIndex);
}

StageSession& PlayScene::session() const {
    return m_services.story.session();
}

StageArtView& PlayScene::art() const {
    return m_services.story.stageArt();
}

void PlayScene::setPhase(Phase phase) {
    m_phase = phase;
    m_phaseMs = 0;
}

void PlayScene::handle(Command command) {
    StoryArt& story = m_services.story;
    switch (m_phase) {
        case Phase::Playing:
            if (command == Command::Pause || command == Command::Quit) {
                story.menu().open(art().frame());
                setPhase(Phase::Menu);
            } else if (!art().wantsPause()) {  // 그림 연출 중에는 블록을 움직이지 않는다
                session().handle(command);
            }
            break;
        case Phase::Menu:
            handleMenu(command);
            break;
        case Phase::MemoryBook:
            story.book().handle(command);
            if (!story.book().isOpen()) {
                setPhase(Phase::Menu);
            }
            break;
        case Phase::Settings:
            story.settings().handle(command);
            m_services.sound.setOn(story.settings().soundOn());
            if (!story.settings().isOpen()) {
                setPhase(Phase::Menu);
            }
            break;
        default:
            break;
    }
}

void PlayScene::handleMenu(Command command) {
    StoryArt& story = m_services.story;
    MenuView& menu = story.menu();
    menu.handle(command);
    switch (menu.choice()) {
        case MenuView::Choice::Resume:
            menu.clearChoice();
            setPhase(Phase::Playing);
            break;
        case MenuView::Choice::MemoryBook:
            menu.clearChoice();
            // 지금 하는 칸을 넘기면 MemoryBookView의 회색 사진 그리기(state 1)가 배열 범위를 넘어 쓴다.
            //     ui 쪽에서 고칠 때까지 -1(없음)을 넘긴다.
            story.book().open(story.remembered(), -1, art().frame());
            setPhase(Phase::MemoryBook);
            break;
        case MenuView::Choice::Settings:
            menu.clearChoice();
            story.settings().open(m_services.sound.isOn(), art().frame());
            setPhase(Phase::Settings);
            break;
        case MenuView::Choice::QuitToTitle:
            menu.clearChoice();
            changeTo(make_unique<PrologueScene>(m_services, 0));  // 새 게임: 집 오프닝부터
            break;
        default:
            break;
    }
}

void PlayScene::update(int dtMs) {
    m_phaseMs += dtMs;
    StoryArt& story = m_services.story;
    switch (m_phase) {
        case Phase::Playing:
            updatePlaying(dtMs);
            break;
        case Phase::Menu:
            story.menu().update(dtMs);
            break;
        case Phase::MemoryBook:
            story.book().update(dtMs);
            break;
        case Phase::Settings:
            story.settings().update(dtMs);
            break;
        case Phase::Failing:
            art().update(dtMs);
            if (m_phaseMs >= config::kStageFailMs) {
                changeTo(make_unique<PlayScene>(m_services, m_stageIndex));  // 재도전: 지금 라운드부터 보드를 비우고 다시
            }
            break;
    }
}

void PlayScene::updatePlaying(int dtMs) {
    StoryArt& story = m_services.story;
    art().update(dtMs);
    if (art().wantsPause()) {
        return;  // 줄 삭제·라운드 시작·라운드 끝 연출 중에는 블록이 멈춘다
    }
    if (story.roundEndPending()) {
        // 라운드 끝 연출까지 끝났다. 찾은 기억을 앨범에 붙이러 간다.
        story.clearRoundEnd();
        if (session().isCleared()) {
            m_services.album.add(session().config().photo);
        }
        changeTo(make_unique<AlbumScene>(m_services, m_stageIndex, story.lastClearedRound()));
        return;
    }
    session().update(dtMs);
    if (session().isFailed()) {
        m_services.sound.play(SoundId::GameOver);
        setPhase(Phase::Failing);
    }
}

void PlayScene::render(Console& console) const {
    const StoryArt& story = m_services.story;
    switch (m_phase) {
        case Phase::Menu:
            story.menu().draw(console);
            break;
        case Phase::MemoryBook:
            story.book().draw(console);
            break;
        case Phase::Settings:
            story.settings().draw(console);
            break;
        default:
            art().draw(console);
            break;
    }
}
