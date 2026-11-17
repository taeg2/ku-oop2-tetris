#pragma once

#include <string>

#include "boss/Boss.h"

// STAGE 2 보스.
// 기믹이 발동하면 내려오던 블록이 갑자기 다른 모양으로 바뀐다.
class ShapeShiftBoss : public Boss {
public:
    ShapeShiftBoss(const std::string& name, int periodMs);

protected:
    void activateGimmick(EffectTarget& target) override;
};
