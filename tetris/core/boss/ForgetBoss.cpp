#include "boss/ForgetBoss.h"

#include <algorithm>
#include <vector>

ForgetBoss::ForgetBoss(const std::string& name, int periodMs, int removeCount, unsigned int seed)
    : Boss(name, periodMs), removeCount(removeCount), random(seed) {}

void ForgetBoss::activateGimmick(EffectTarget& target) {
    std::vector<Point> filledCells = target.getBoard().getFilledCells();
    if (filledCells.empty()) {
        return;
    }

    // 쌓인 칸을 섞은 뒤 앞에서부터 removeCount개를 지운다
    std::shuffle(filledCells.begin(), filledCells.end(), random);
    const int count = std::min(removeCount, static_cast<int>(filledCells.size()));
    filledCells.resize(count);
    target.removeCells(filledCells);
}
