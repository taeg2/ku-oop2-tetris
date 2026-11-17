#include "medicine/BluePill.h"

BluePill::BluePill(const std::string& name, int durationMs)
    : Medicine(name), durationMs(durationMs) {}

int BluePill::getDurationMs() const {
    return durationMs;
}

std::unique_ptr<Medicine> BluePill::clone() const {
    return std::make_unique<BluePill>(*this);
}

void BluePill::applyTo(EffectTarget& target) {
    target.slowDown(durationMs);
}
