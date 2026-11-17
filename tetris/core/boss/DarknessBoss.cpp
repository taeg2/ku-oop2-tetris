#include "boss/DarknessBoss.h"

#include <algorithm>

DarknessBoss::DarknessBoss(const std::string& name, int periodMs, int darkRowCount, int darkDurationMs,
                           unsigned int seed)
    : Boss(name, periodMs),
      darkRowCount(darkRowCount),
      darkDurationMs(darkDurationMs),
      darkTopRow(0),
      darkRemainingMs(0),
      random(seed) {}

bool DarknessBoss::hidesCell(const Point& cell) const {
    return isActive()
        && darkRemainingMs > 0
        && cell.getRow() >= darkTopRow
        && cell.getRow() < darkTopRow + darkRowCount;
}

int DarknessBoss::getDarkRemainingMs() const {
    return darkRemainingMs;
}

void DarknessBoss::activateGimmick(EffectTarget& target) {
    // 가린 줄이 보드 안에 모두 들어가도록 맨 위 줄을 고른다
    const int lastTopRow = std::max(0, target.getBoard().getHeight() - darkRowCount);
    std::uniform_int_distribution<int> topRowDistribution(0, lastTopRow);
    darkTopRow = topRowDistribution(random);
    darkRemainingMs = darkDurationMs;
}

void DarknessBoss::onUpdate(int elapsedMs) {
    darkRemainingMs = std::max(0, darkRemainingMs - elapsedMs);
}

void DarknessBoss::resetGimmick() {
    darkRemainingMs = 0;
}
