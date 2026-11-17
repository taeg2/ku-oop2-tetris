#pragma once

#include <random>
#include <string>

#include "boss/Boss.h"

// STAGE 1 보스 (단기기억상실).
// 기믹이 발동하면 보드의 가로 몇 줄이 일정 시간 동안 암전되어, 그 안의 블록이 화면에서 가려진다.
// 보드 자체는 바꾸지 않고, 화면 담당 클래스가 hidesCell()을 보고 가린다.
class DarknessBoss : public Boss {
public:
    // darkRowCount: 한 번에 가리는 줄 수
    // darkDurationMs: 가려진 상태가 유지되는 시간
    // seed: 가릴 위치를 고르는 난수 시드
    DarknessBoss(const std::string& name, int periodMs, int darkRowCount, int darkDurationMs,
                 unsigned int seed = std::random_device()());

    bool hidesCell(const Point& cell) const override;

    // 암전이 풀리기까지 남은 시간. 암전 중이 아니면 0이다.
    int getDarkRemainingMs() const;

protected:
    void activateGimmick(EffectTarget& target) override;
    void onUpdate(int elapsedMs) override;
    void resetGimmick() override;

private:
    int darkRowCount;
    int darkDurationMs;
    int darkTopRow;
    int darkRemainingMs;
    std::mt19937 random;
};
