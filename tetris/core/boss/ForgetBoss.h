#pragma once

#include <random>
#include <string>

#include "boss/Boss.h"

// STAGE 3 보스 (까먹음).
// 기믹이 발동하면 보드에 쌓인 블록 중 몇 칸을 무작위로 지운다. 위의 블록은 내려오지 않는다.
class ForgetBoss : public Boss {
public:
    // removeCount: 한 번에 지우는 칸 수
    // seed: 지울 칸을 고르는 난수 시드
    ForgetBoss(const std::string& name, int periodMs, int removeCount,
               unsigned int seed = std::random_device()());

protected:
    void activateGimmick(EffectTarget& target) override;

private:
    int removeCount;
    std::mt19937 random;
};
