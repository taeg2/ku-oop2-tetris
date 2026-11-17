#pragma once
#include "medicine/Pill.h"

// 빨간 약: 보드 맨 아래 한 줄을 바로 없앤다. 맨 아래 줄이 비어 있으면 쓸 수 없다.
class RedPill : public Pill {
public:
    RedPill();
    bool canUse(const PlayContext& ctx) const override;
    void use(PlayContext& ctx) override;
    std::string name() const override { return "빨간 약"; }
};
