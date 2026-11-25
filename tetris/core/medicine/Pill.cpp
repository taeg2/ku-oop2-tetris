#include "medicine/Pill.h"
#include "medicine/BluePill.h"
#include "common/GameConfig.h"
#include "medicine/RedPill.h"
using namespace std;

bool Pill::canUse(const PlayContext&) const {
    return true;
}

unique_ptr<Pill> Pill::create(PillType type) {
    switch (type) {
        case PillType::Blue:
            return make_unique<BluePill>(config::kBluePillSlowFactor, config::kBluePillDurationMs);
        case PillType::Red:
            return make_unique<RedPill>();
    }
    return nullptr;
}
