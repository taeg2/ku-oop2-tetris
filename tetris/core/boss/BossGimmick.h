#pragma once
#include <string>
#include "play/PlayContext.h"
using namespace std;

// 보스 방해 기믹의 추상 클래스. 필요한 시점(훅)만 골라 재정의한다.
//     새 보스를 만들 때 Board나 StageSession 코드는 바꾸지 않는다.
class BossGimmick {
public:
    virtual ~BossGimmick() = default;

    virtual void onActivate(PlayContext& ctx);
    virtual void onDeactivate(PlayContext& ctx);
    virtual void onTick(PlayContext& ctx, int dtMs);
    virtual void onPieceSpawn(PlayContext& ctx);
    virtual string statusText() const = 0;  // HUD에 보여 줄 한 줄
};
