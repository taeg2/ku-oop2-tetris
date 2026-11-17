#include "medicine/Pill.h"
#include "medicine/BluePill.h"
#include "common/GameConfig.h"
#include "medicine/RedPill.h"

bool Pill::canUse(const PlayContext&) const {
    return true;
}

std::unique_ptr<Pill> Pill::create(PillType type) {
    switch (type) {
        case PillType::Blue:
            return std::make_unique<BluePill>(config::kBluePillSlowFactor, config::kBluePillDurationMs);
        case PillType::Red:
            return std::make_unique<RedPill>();
    }
    return nullptr;
}
