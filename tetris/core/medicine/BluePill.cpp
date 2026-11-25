#include "medicine/BluePill.h"
#include "play/EventBus.h"
#include "play/SpeedController.h"

BluePill::BluePill(double slowFactor, int durationMs)
    : Pill(PillType::Blue), m_slowFactor(slowFactor), m_durationMs(durationMs) {}

void BluePill::use(PlayContext& ctx, Inventory&) {
    ctx.speed.applySlow(m_slowFactor, m_durationMs);
    ctx.bus.publish(GameEvent::Type::PillUsed, static_cast<int>(m_type));
}
