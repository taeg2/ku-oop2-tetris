#pragma once
#include "play/StageSession.h"
#include "ui/View.h"

// 오른쪽 4분할 사진 틀. 채운 조각은 컬러로, 못 찾은 조각은 어둡게 그린다.
//     새 조각이 채워지면 잠시 깜빡이고, 4/4가 되면 경계선이 사라지며 한 장으로 합쳐진다.
class PhotoFrameView : public View {
public:
    PhotoFrameView(Point origin, const StageSession& session);
    void update(int dtMs) override;
    void draw(Console& console) const override;

    static constexpr int kWidthColumns = 54;
    static constexpr int kHeightRows = 19;

    Point center() const;

private:
    Point quadrantOrigin(int index) const;
    void drawQuadrant(Console& console, int index) const;

    const StageSession& m_session;
    int m_knownPieces = 0;
    int m_highlightIndex = -1;
    int m_highlightLeftMs = 0;
};
