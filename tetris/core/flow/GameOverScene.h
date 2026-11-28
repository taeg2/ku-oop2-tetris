#pragma once
#include "flow/Scene.h"
#include "ui/GameOverView.h"

class GameOverScene : public Scene {
public:
    GameOverScene(GameServices& services, int stageIndex);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    int m_stageIndex;
    GameOverView m_view;
};
