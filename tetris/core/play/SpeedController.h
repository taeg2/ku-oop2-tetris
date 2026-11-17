#pragma once
#include "play/IGameEventListener.h"

// 블록 낙하 간격 관리. 사진 조각이 채워지면 빨라지고, 파란 약을 먹으면 잠시 느려진다.
class SpeedController : public IGameEventListener {
public:
    explicit SpeedController(int baseIntervalMs);

    void onEvent(const GameEvent& event) override;
    void speedUp();
    void speedUp(int steps);
    void applySlow(double factor, int durationMs);
    void update(int dtMs);
    void reset(int baseIntervalMs);

    int dropIntervalMs() const;
    int level() const { return m_level; }
    bool isSlowed() const { return m_slowLeftMs > 0; }
    int slowLeftMs() const { return m_slowLeftMs; }

private:
    int m_baseMs;
    int m_level = 0;
    double m_slowFactor = 1.0;
    int m_slowLeftMs = 0;
};
