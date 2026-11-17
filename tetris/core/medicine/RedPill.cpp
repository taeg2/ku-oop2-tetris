#include "medicine/RedPill.h"

RedPill::RedPill(const std::string& name) : Medicine(name) {}

std::unique_ptr<Medicine> RedPill::clone() const {
    return std::make_unique<RedPill>(*this);
}

void RedPill::applyTo(EffectTarget& target) {
    target.removeBottomLine();
}
