#pragma once
#include "flow/Scene.h"

// 실패 화면. 재도전하면 해당 스테이지를 시작 시점부터 다시 한다. (기획안 3장 실패 및 재시작)
class GameOverScene : public Scene {
public:
    GameOverScene(GameServices& services, int stageIndex);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    int m_stageIndex;
    int m_timeMs = 0;
};
