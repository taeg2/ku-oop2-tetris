#include "boss/PeriodicGimmick.h"

PeriodicGimmick::PeriodicGimmick(int intervalMs, int warningMs)
    : m_intervalMs(intervalMs), m_warningMs(warningMs) {}

void PeriodicGimmick::onActivate(PlayContext&) {
    m_elapsedMs = 0;
    m_warned = false;
}

void PeriodicGimmick::onTick(PlayContext& ctx, int dtMs) {
    onUpdate(ctx, dtMs);
    m_elapsedMs += dtMs;
    if (!m_warned && m_warningMs > 0 && remainingMs() <= m_warningMs) {
        m_warned = true;
        onWarning(ctx);
    }
    if (m_elapsedMs >= m_intervalMs) {
        m_elapsedMs = 0;
        m_warned = false;
        trigger(ctx);
    }
}

void PeriodicGimmick::onWarning(PlayContext&) {}
void PeriodicGimmick::onUpdate(PlayContext&, int) {}
