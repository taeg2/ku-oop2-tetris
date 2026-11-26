#pragma once
#include <memory>
#include <string>
#include "boss/BossGimmick.h"
#include "play/IGameEventListener.h"
using namespace std;

// 스테이지 보스. 사진이 2/4가 되면 등장하고 4/4가 되면 떠난다 (이벤트로 반응).
//     "무엇을 방해하는지"는 소유한 BossGimmick 객체에 맡긴다 (합성 + 다형성).
class Boss : public IGameEventListener {
public:
    Boss(string name, string symptom, string entranceLine, unique_ptr<BossGimmick> gimmick);

    void onEvent(const GameEvent& event) override;
    void update(PlayContext& ctx, int dtMs);
    void onPieceSpawn(PlayContext& ctx);

    bool isActive() const { return m_active; }
    const string& name() const { return m_name; }
    const string& symptom() const { return m_symptom; }
    const string& entranceLine() const { return m_entranceLine; }
    string statusText() const;

private:
    enum class Pending { None, Activate, Deactivate };

    string m_name;
    string m_symptom;
    string m_entranceLine;
    unique_ptr<BossGimmick> m_gimmick;
    bool m_active = false;
    Pending m_pending = Pending::None;
};
