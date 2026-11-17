#pragma once
#include "boss/BossGimmick.h"

// "일정 주기마다 발동"하는 기믹의 공통 뼈대 (템플릿 메서드 패턴).
//     시간 계산(onTick)은 여기서 고정하고, 자식은 trigger()에서 무엇을 할지만 정한다.
class PeriodicGimmick : public BossGimmick {
public:
    PeriodicGimmick(int intervalMs, int warningMs);

    void onActivate(PlayContext& ctx) override;
    void onTick(PlayContext& ctx, int dtMs) final;

protected:
    virtual void trigger(PlayContext& ctx) = 0;
    virtual void onWarning(PlayContext& ctx);       // 발동 warningMs 전에 한 번
    virtual void onUpdate(PlayContext& ctx, int dtMs);  // 매 프레임
    int remainingMs() const { return m_intervalMs - m_elapsedMs; }
    bool isWarning() const { return m_warned; }
    static int secondsLeft(int ms) { return (ms + 999) / 1000; }

private:
    int m_intervalMs;
    int m_warningMs;
    int m_elapsedMs = 0;
    bool m_warned = false;
};
