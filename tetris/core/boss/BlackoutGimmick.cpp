#include "boss/BlackoutGimmick.h"
#include <algorithm>
#include "board/Board.h"
#include "play/EventBus.h"
using namespace std;

BlackoutGimmick::BlackoutGimmick(int intervalMs, int durationMs, int areaWidth, int areaHeight)
    : PeriodicGimmick(intervalMs, 0), m_durationMs(durationMs), m_areaWidth(areaWidth), m_areaHeight(areaHeight) {}

void BlackoutGimmick::trigger(PlayContext& ctx) {
    uniform_int_distribution<int> xDist(0, Board::kWidth - m_areaWidth);
    uniform_int_distribution<int> yDist(Board::kHeight / 3, Board::kHeight - m_areaHeight);
    const Rect area{xDist(ctx.rng), yDist(ctx.rng), m_areaWidth, m_areaHeight};
    ctx.board.setHidden(false);
    ctx.board.setHidden(area, true);
    m_activeLeftMs = m_durationMs;
    ctx.bus.publish(GameEvent::Type::BossTriggered);
}

void BlackoutGimmick::onUpdate(PlayContext& ctx, int dtMs) {
    if (m_activeLeftMs <= 0) {
        return;
    }
    m_activeLeftMs = max(0, m_activeLeftMs - dtMs);
    if (m_activeLeftMs == 0) {
        ctx.board.setHidden(false);
    }
}

void BlackoutGimmick::onDeactivate(PlayContext& ctx) {
    m_activeLeftMs = 0;
    ctx.board.setHidden(false);
}

string BlackoutGimmick::statusText() const {
    if (m_activeLeftMs > 0) {
        return "안개 걷힘까지 " + to_string(secondsLeft(m_activeLeftMs)) + "초";
    }
    return "다음 안개까지 " + to_string(secondsLeft(remainingMs())) + "초";
}
