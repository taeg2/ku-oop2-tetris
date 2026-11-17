#pragma once
#include "play/StageSession.h"
#include "ui/View.h"

// 상태 표시: 스테이지, 사진 진행 n/4, 다음 조각까지 남은 줄, 속도, 약, 보스 상태
class HudView : public View {
public:
    HudView(Point origin, const StageSession& session);
    void draw(Console& console) const override;

    static constexpr int kWidthColumns = 17;

private:
    void drawPills(Console& console, int y) const;
    void drawBoss(Console& console, int y) const;

    const StageSession& m_session;
};
