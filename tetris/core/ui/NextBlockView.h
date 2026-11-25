#pragma once
#include "play/StageSession.h"
#include "ui/View.h"

// 다음 블록 미리보기 상자
class NextBlockView : public View {
public:
    NextBlockView(Point origin, const StageSession& session);
    void draw(Console& console) const override;

    inline static const int kWidthColumns = 16;
    inline static const int kHeightRows = 6;

private:
    const StageSession& m_session;
};
