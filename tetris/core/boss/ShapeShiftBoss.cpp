#include "boss/ShapeShiftBoss.h"

ShapeShiftBoss::ShapeShiftBoss(const std::string& name, int periodMs)
    : Boss(name, periodMs) {}

void ShapeShiftBoss::activateGimmick(EffectTarget& target) {
    target.transformCurrentPiece();
}
