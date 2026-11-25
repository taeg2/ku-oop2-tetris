#include "boss/ForgetGimmick.h"
#include <algorithm>
#include "board/Board.h"
#include "play/EventBus.h"
using namespace std;

ForgetGimmick::ForgetGimmick(int intervalMs, int warningMs, int cellsPerTrigger)
    : PeriodicGimmick(intervalMs, warningMs), m_cellsPerTrigger(cellsPerTrigger) {}

void ForgetGimmick::onWarning(PlayContext& ctx) {
    vector<Point> filled = ctx.board.filledCells();
    shuffle(filled.begin(), filled.end(), ctx.rng);
    const auto count = min<size_t>(filled.size(), static_cast<size_t>(m_cellsPerTrigger));
    m_targets.assign(filled.begin(), filled.begin() + static_cast<ptrdiff_t>(count));
    ctx.board.setMarked(m_targets, true);
}

void ForgetGimmick::trigger(PlayContext& ctx) {
    if (m_targets.empty()) {
        return;
    }
    ctx.board.removeCells(m_targets);
    ctx.board.setMarked(false);
    m_targets.clear();
    ctx.bus.publish(GameEvent::Type::BossTriggered);
}

void ForgetGimmick::onDeactivate(PlayContext& ctx) {
    ctx.board.setMarked(false);
    m_targets.clear();
}

string ForgetGimmick::statusText() const {
    if (isWarning() && !m_targets.empty()) {
        return "깜빡이는 칸 소멸!";
    }
    return "다음 망각까지 " + to_string(secondsLeft(remainingMs())) + "초";
}
