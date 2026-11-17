#include "play/FallSpeed.h"

#include <algorithm>

FallSpeed::FallSpeed(int startIntervalMs, int stepMs, int minIntervalMs, int slowMultiplier)
    : startIntervalMs(startIntervalMs),
      stepMs(stepMs),
      minIntervalMs(minIntervalMs),
      slowMultiplier(slowMultiplier),
      level(0),
      slowRemainingMs(0) {}

int FallSpeed::getFallIntervalMs() const {
    const int interval = std::max(minIntervalMs, startIntervalMs - level * stepMs);
    if (slowRemainingMs > 0) {
        return interval * slowMultiplier;
    }
    return interval;
}

int FallSpeed::getLevel() const {
    return level;
}

int FallSpeed::getSlowRemainingMs() const {
    return slowRemainingMs;
}

void FallSpeed::speedUp() {
    ++level;
}

void FallSpeed::slowDown(int durationMs) {
    slowRemainingMs = durationMs;
}

void FallSpeed::update(int elapsedMs) {
    slowRemainingMs = std::max(0, slowRemainingMs - elapsedMs);
}

void FallSpeed::reset() {
    level = 0;
    slowRemainingMs = 0;
}
