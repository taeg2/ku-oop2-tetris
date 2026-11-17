#include "boss/Boss.h"
#include "common/ConfigException.h"

Boss::Boss(std::string name, std::string symptom, std::string entranceLine, std::unique_ptr<BossGimmick> gimmick)
    : m_name(std::move(name)), m_symptom(std::move(symptom)), m_entranceLine(std::move(entranceLine)),
      m_gimmick(std::move(gimmick)) {
    if (!m_gimmick) {
        throw ConfigException("보스 '" + m_name + "'에 기믹이 없습니다");
    }
}

void Boss::onEvent(const GameEvent& event) {
    // 이벤트 시점에는 보드에 손대지 않고 표시만 해 두었다가, 다음 update()에서 처리한다.
    if (event.type == GameEvent::Type::PhotoHalf) {
        m_pending = Pending::Activate;
        m_active = true;
    } else if (event.type == GameEvent::Type::PhotoComplete) {
        m_pending = Pending::Deactivate;
        m_active = false;
    }
}

void Boss::update(PlayContext& ctx, int dtMs) {
    if (m_pending == Pending::Activate) {
        m_gimmick->onActivate(ctx);
    } else if (m_pending == Pending::Deactivate) {
        m_gimmick->onDeactivate(ctx);
    }
    m_pending = Pending::None;

    if (m_active) {
        m_gimmick->onTick(ctx, dtMs);
    }
}

void Boss::onPieceSpawn(PlayContext& ctx) {
    if (m_active) {
        m_gimmick->onPieceSpawn(ctx);
    }
}

std::string Boss::statusText() const {
    return m_active ? m_gimmick->statusText() : "";
}
