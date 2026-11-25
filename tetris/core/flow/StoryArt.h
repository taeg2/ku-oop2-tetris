#pragma once
#include <memory>
#include <random>
#include <vector>
#include "play/IGameEventListener.h"
#include "play/StageSession.h"
#include "ui/AlbumView.h"
#include "ui/EndingView.h"
#include "ui/MemoryBookView.h"
#include "ui/MenuView.h"
#include "ui/PrologueView.h"
#include "ui/SettingsView.h"
#include "ui/StageArtView.h"

class ISoundPlayer;
class StageCatalog;

// 게임 한 판 동안 장면이 바뀌어도 이어져야 하는 화면과 진행 상태를 들고 있다.
//     - ui의 그림 View들 (집, 앨범, 라운드 그림, 메뉴, 설정, 기억의 책, 엔딩)
//     - 라운드 그림이 계속 읽는 StageSession (스테이지마다 같은 자리에 새로 만든다)
//     - 12개 라운드 중 어디까지 기억했는지
// 장면(Scene)은 이 객체를 GameServices로 빌려 쓰고, 다음 장면으로 넘어가도 그림 상태가 유지된다.
class StoryArt : public IGameEventListener {
public:
    inline static const int kRoundsPerStage = 4;

    StoryArt(const StageCatalog& catalog, ISoundPlayer& sound, std::mt19937& rng);

    void restart();                  // 새 게임: 모든 그림과 진행을 처음 상태로
    void beginStage(int stageIndex); // 이 스테이지를 플레이할 준비 (진행 중이면 이어서, 실패했으면 보드만 비움)
    void clearRoundEnd();            // 라운드 끝을 처리했다고 표시
    void onEvent(const GameEvent& event) override;

    bool hasSession() const { return m_session != nullptr; }
    StageSession& session() { return *m_session; }
    const StageSession& session() const { return *m_session; }
    int stageIndex() const { return m_stageIndex; }

    bool roundEndPending() const { return m_roundEndPending; }
    int lastClearedRound() const { return m_lastCleared; }  // 이 스테이지에서 방금 끝낸 칸 (0~3)
    const std::vector<bool>& remembered() const { return m_remembered; }

    StageArtView& stageArt() { return *m_stageArt; }
    const StageArtView& stageArt() const { return *m_stageArt; }
    PrologueView& house() { return *m_house; }
    const PrologueView& house() const { return *m_house; }
    AlbumView& album() { return *m_album; }
    const AlbumView& album() const { return *m_album; }
    MenuView& menu() { return *m_menu; }
    const MenuView& menu() const { return *m_menu; }
    SettingsView& settings() { return *m_settings; }
    const SettingsView& settings() const { return *m_settings; }
    MemoryBookView& book() { return *m_book; }
    const MemoryBookView& book() const { return *m_book; }
    EndingView& ending() { return *m_ending; }
    const EndingView& ending() const { return *m_ending; }

private:
    const StageCatalog& m_catalog;
    ISoundPlayer& m_sound;
    std::mt19937& m_rng;

    // StageArtView가 세션을 참조로 들고 있으므로, 세션은 한 판 동안 하나만 만들고 changeStage()로 스테이지를 바꾼다.
    std::unique_ptr<StageSession> m_session;
    int m_stageIndex = -1;
    int m_roundInStage = 0;
    int m_lastCleared = -1;
    bool m_roundEndPending = false;
    std::vector<bool> m_remembered;

    std::unique_ptr<StageArtView> m_stageArt;
    std::unique_ptr<PrologueView> m_house;
    std::unique_ptr<AlbumView> m_album;
    std::unique_ptr<MenuView> m_menu;
    std::unique_ptr<SettingsView> m_settings;
    std::unique_ptr<MemoryBookView> m_book;
    std::unique_ptr<EndingView> m_ending;
};
