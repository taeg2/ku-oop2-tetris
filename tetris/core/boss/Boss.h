#pragma once
#include <memory>
#include <string>
#include "boss/BossGimmick.h"
#include "play/IGameEventListener.h"

// 스테이지 보스. 사진이 2/4가 되면 등장하고 4/4가 되면 떠난다 (이벤트로 반응).
//     "무엇을 방해하는지"는 소유한 BossGimmick 객체에 맡긴다 (합성 + 다형성).
class Boss : public IGameEventListener {
public:
    Boss(std::string name, std::string symptom, std::string entranceLine, std::unique_ptr<BossGimmick> gimmick);

    void onEvent(const GameEvent& event) override;
    void update(PlayContext& ctx, int dtMs);
    void onPieceSpawn(PlayContext& ctx);

    bool isActive() const { return m_active; }
    const std::string& name() const { return m_name; }
    const std::string& symptom() const { return m_symptom; }
    const std::string& entranceLine() const { return m_entranceLine; }
    std::string statusText() const;

private:
    enum class Pending { None, Activate, Deactivate };

    std::string m_name;
    std::string m_symptom;
    std::string m_entranceLine;
    std::unique_ptr<BossGimmick> m_gimmick;
    bool m_active = false;
    Pending m_pending = Pending::None;
};
