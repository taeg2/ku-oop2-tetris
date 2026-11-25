#pragma once
#include "medicine/Pill.h"
using namespace std;

// 파란 약: 일정 시간 동안 블록 낙하 속도를 늦춘다.
class BluePill : public Pill {
public:
    BluePill(double slowFactor, int durationMs);
    void use(PlayContext& ctx, Inventory& bag) override;
    string name() const override { return "파란 약"; }

private:
    double m_slowFactor;
    int m_durationMs;
};
