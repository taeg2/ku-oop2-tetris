#pragma once
#include "play/StageSession.h"
#include "ui/View.h"

// 다음 블록 미리보기 상자
class NextBlockView : public View {
public:
    NextBlockView(Point origin, const StageSession& session);
    void draw(Console& console) const override;

    static constexpr int kWidthColumns = 16;
    static constexpr int kHeightRows = 6;

private:
    const StageSession& m_session;
};
