#include "ui/NextBlockView.h"
#include <algorithm>

NextBlockView::NextBlockView(Point origin, const StageSession& session) : View(origin), m_session(session) {}

void NextBlockView::draw(Console& console) const {
    console.frame({m_origin.x, m_origin.y, kWidthColumns, kHeightRows}, Color::DarkGray);
    console.draw({m_origin.x + 4, m_origin.y}, "다음 블록", Color::White, Color::DarkGray);

    const Tetromino next(m_session.nextType());
    const Tetromino::Cells cells = next.localCells();
    int minX = 4, maxX = 0, minY = 4, maxY = 0;
    for (Point p : cells) {
        minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
    }
    const int pieceCols = (maxX - minX + 1) * 2;
    const int pieceRows = maxY - minY + 1;
    const int left = m_origin.x + (kWidthColumns - pieceCols) / 2;
    const int top = m_origin.y + 1 + (kHeightRows - 2 - pieceRows) / 2;
    for (Point p : cells) {
        console.draw({left + (p.x - minX) * 2, top + (p.y - minY)}, "  ", next.color(), next.color());
    }
}
