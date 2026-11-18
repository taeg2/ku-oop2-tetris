#include "play/SpeedController.h"
#include <algorithm>
#include <cmath>
#include "common/GameConfig.h"

SpeedController::SpeedController(int baseIntervalMs) : m_baseMs(baseIntervalMs) {}

void SpeedController::onEvent(const GameEvent& event) {
    if (event.type == GameEvent::Type::PieceFilled) {
        speedUp();
    }
}

void SpeedController::speedUp() {
    speedUp(1);
}

void SpeedController::speedUp(int steps) {
    m_level += std::max(0, steps);
}

void SpeedController::applySlow(double factor, int durationMs) {
    m_slowFactor = factor;
    m_slowLeftMs = durationMs;  // 효과 중에 다시 먹으면 시간이 처음부터 다시 흐른다
}

void SpeedController::update(int dtMs) {
    if (m_slowLeftMs > 0) {
        m_slowLeftMs = std::max(0, m_slowLeftMs - dtMs);
    }
}

void SpeedController::reset(int baseIntervalMs) {
    m_baseMs = baseIntervalMs;
    m_level = 0;
    m_slowFactor = 1.0;
    m_slowLeftMs = 0;
}

int SpeedController::dropIntervalMs() const {
    const double scaled = m_baseMs * std::pow(config::kSpeedUpFactor, m_level);
    const int interval = std::max(config::kMinDropIntervalMs, static_cast<int>(scaled));
    return isSlowed() ? static_cast<int>(interval * m_slowFactor) : interval;
}
