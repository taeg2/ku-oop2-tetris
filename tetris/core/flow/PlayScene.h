#pragma once
#include <memory>
#include <vector>
#include "play/IGameEventListener.h"
#include "play/StageSession.h"
#include "ui/ParticleView.h"
#include "ui/TextBoxView.h"
#include "ui/View.h"
#include "flow/Scene.h"

// 스테이지 플레이 화면. StageSession(규칙)과 View들(화면)을 묶고,
//     세션의 이벤트를 구독해 메시지와 조각 연출을 띄운다. (Scene이면서 IGameEventListener)
class PlayScene : public Scene, public IGameEventListener {
public:
    PlayScene(GameServices& services, int stageIndex);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;
    void onEvent(const GameEvent& event) override;

private:
    enum class Phase { Intro, Playing, Paused, Completing, Failing };

    void setPhase(Phase phase);
    void showMessage(const std::string& speaker, const std::string& text);
    void spawnClearParticles(int topRow, int rowCount);
    void renderHeader(Console& console) const;
    void renderOverlay(Console& console) const;

    int m_stageIndex;
    const StageConfig& m_config;
    StageSession m_session;
    std::vector<std::unique_ptr<View>> m_views;
    TextBoxView m_message;
    ParticleView m_particles;
    Phase m_phase = Phase::Intro;
    int m_phaseMs = 0;
};
