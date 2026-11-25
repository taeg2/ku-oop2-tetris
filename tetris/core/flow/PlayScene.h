#pragma once
#include "flow/Scene.h"

class StageSession;
class StageArtView;

// 스테이지 플레이 화면. 규칙은 StageSession, 그림은 ui의 StageArtView(라운드 그림)가 맡는다.
//     둘 다 StoryArt가 들고 있어서, 앨범을 다녀와도 같은 스테이지를 이어서 한다.
//     라운드 하나(사진 조각 하나)가 끝나면 끝 연출을 본 뒤 앨범 장면으로 간다.
//     Esc를 누르면 메뉴(MenuView)를 띄우고, 메뉴에서 기억의 책·설정을 연다.
class PlayScene : public Scene {
public:
    PlayScene(GameServices& services, int stageIndex);

    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    enum class Phase { Playing, Menu, MemoryBook, Settings, Failing };

    void setPhase(Phase phase);
    void handleMenu(Command command);
    void updatePlaying(int dtMs);
    StageSession& session() const;
    StageArtView& art() const;

    int m_stageIndex;
    Phase m_phase = Phase::Playing;
    int m_phaseMs = 0;
};
