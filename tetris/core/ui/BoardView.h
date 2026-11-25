#pragma once
#include "play/StageSession.h"
#include "ui/View.h"

// 테트리스 보드 그리기: 쌓인 블록, 떨어지는 블록, 착지 그림자, 안개(가림), 지워질 칸 깜빡임
class BoardView : public View {
public:
    BoardView(Point origin, const StageSession& session);
    void draw(Console& console) const override;

    inline static const int kWidthColumns = (Board::kWidth + 2) * 2;
    inline static const int kHeightRows = Board::kHeight + 2;

private:
    Point toScreen(Point cell) const;
    void drawCell(Console& console, Point cell) const;
    void drawPiece(Console& console, const Tetromino& piece, bool ghost) const;

    const StageSession& m_session;
};
