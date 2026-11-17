#include "boss/Boss.h"

#include <stdexcept>

Boss::Boss(const std::string& name, int periodMs)
    : name(name), periodMs(periodMs), timerMs(0), active(false) {
    if (periodMs < 1) {
        throw std::invalid_argument("Boss: 기믹 주기는 1ms 이상이어야 한다");
    }
}

Boss::~Boss() = default;

std::string Boss::getName() const {
    return name;
}

bool Boss::isActive() const {
    return active;
}

int Boss::getMsUntilNextGimmick() const {
    if (!active) {
        return 0;
    }
    return periodMs - timerMs;
}

bool Boss::hidesCell(const Point& /*cell*/) const {
    return false;
}

void Boss::appear() {
    active = true;
    timerMs = 0;
    resetGimmick();
}

void Boss::disappear() {
    active = false;
    timerMs = 0;
    resetGimmick();
}

void Boss::update(int elapsedMs, EffectTarget& target) {
    if (!active) {
        return;
    }
    onUpdate(elapsedMs);
    timerMs += elapsedMs;
    while (timerMs >= periodMs) {
        timerMs -= periodMs;
        activateGimmick(target);
    }
}

void Boss::onUpdate(int /*elapsedMs*/) {}

void Boss::resetGimmick() {}
