#pragma once

#include <memory>
#include <string>

#include "medicine/Medicine.h"

// 파란 약 (가칭). 블록 낙하 속도를 일정 시간 동안 느리게 한다.
// 얼마나 느려지는지는 FallSpeed의 slowMultiplier가 정한다.
class BluePill : public Medicine {
public:
    // durationMs: 느려진 상태가 유지되는 시간 (기획안 미정이라 main에서 넘겨준다)
    BluePill(const std::string& name, int durationMs);

    int getDurationMs() const;
    std::unique_ptr<Medicine> clone() const override;

    void applyTo(EffectTarget& target) override;

private:
    int durationMs;
};
