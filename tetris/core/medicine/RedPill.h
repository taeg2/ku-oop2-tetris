#pragma once

#include <memory>
#include <string>

#include "medicine/Medicine.h"

// 빨간 약 (가칭). 보드 맨 아래 한 줄을 즉시 없앤다.
// 없앤 줄을 사진 조각 누적에 포함할지는 기획안에서 미정이라, 지금은 포함하지 않는다.
class RedPill : public Medicine {
public:
    explicit RedPill(const std::string& name);

    std::unique_ptr<Medicine> clone() const override;

    void applyTo(EffectTarget& target) override;
};
