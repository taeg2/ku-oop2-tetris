#pragma once
#include "common/Point.h"
#include "boss/PeriodicGimmick.h"
using namespace std;

// STAGE 1 보스 "짙은 안개" - 단기기억상실.
//     주기마다 보드 일부를 안개로 가려, 방금 쌓은 블록과 떨어지는 블록이 보이지 않게 한다.
class BlackoutGimmick : public PeriodicGimmick {
public:
    BlackoutGimmick(int intervalMs, int durationMs, int areaWidth, int areaHeight);

    void onDeactivate(PlayContext& ctx) override;
    string statusText() const override;

protected:
    void trigger(PlayContext& ctx) override;
    void onUpdate(PlayContext& ctx, int dtMs) override;

private:
    int m_durationMs;
    int m_areaWidth;
    int m_areaHeight;
    int m_activeLeftMs = 0;
};
