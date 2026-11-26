#pragma once
#include "medicine/Pill.h"
using namespace std;

// 빨간 약: 보드 맨 아래 한 줄을 바로 없앤다. 맨 아래 줄이 비어 있으면 쓸 수 없다.
//     줄을 채워 지울 때처럼, 지운 줄에 붙어 있던 약은 가방에 넣는다.
class RedPill : public Pill {
public:
    RedPill();
    bool canUse(const PlayContext& ctx) const override;
    void use(PlayContext& ctx, Inventory& bag) override;
    string name() const override { return "빨간 약"; }
};
