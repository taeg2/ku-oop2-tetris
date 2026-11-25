#include "ui/BoardView.h"
#include <algorithm>

BoardView::BoardView(Point origin, const StageSession& session) : View(origin), m_session(session) {}

Point BoardView::toScreen(Point cell) const {
    return {m_origin.x + 2 + cell.x * 2, m_origin.y + 1 + cell.y};
}

void BoardView::draw(Console& console) const {
    const Color border = m_session.boss().isActive() ? Color::DarkRed : Color::DarkGray;
    console.frame({m_origin.x, m_origin.y, kWidthColumns, kHeightRows}, border);

    for (int y = 0; y < Board::kHeight; ++y) {
        for (int x = 0; x < Board::kWidth; ++x) {
            drawCell(console, {x, y});
        }
    }
    if (m_session.hasActivePiece()) {
        drawPiece(console, m_session.ghost(), true);
        drawPiece(console, m_session.current(), false);
    }
}

void BoardView::drawCell(Console& console, Point cell) const {
    const Board& board = m_session.board();
    const Point pos = toScreen(cell);

    if (board.isHidden(cell)) {
        const bool wisp = (cell.x + cell.y + m_timeMs / 300) % 4 == 0;
        console.draw(pos, wisp ? "~ " : "  ", Color::Gray, Color::DarkGray);
        return;
    }
    const auto& rows = m_session.flashingRows();
    if (std::find(rows.begin(), rows.end(), cell.y) != rows.end()) {
        console.draw(pos, "  ", Color::White, blink(70) ? Color::White : Color::Yellow);
        return;
    }
    const Cell& data = board.cellAt(cell);
    if (data.isFilled() && board.isMarked(cell) && blink(150)) {
        console.draw(pos, "  ", Color::White, Color::White);
        return;
    }
    if (data.isFilled()) {
        console.draw(pos, data);
        return;
    }
    console.draw(pos, " .", Color::DarkGray, Color::Black);
}

void BoardView::drawPiece(Console& console, const Tetromino& piece, bool ghost) const {
    const Board& board = m_session.board();
    const Tetromino::Cells cells = piece.cells();
    for (int i = 0; i < Tetromino::kCellCount; ++i) {
        const Point cell = cells[i];
        if (!board.isInside(cell) || board.isHidden(cell)) {
            continue;  // 보드 위쪽이거나 안개 속이면 보이지 않는다
        }
        const Point pos = toScreen(cell);
        if (ghost) {
            if (!board.cellAt(cell).isFilled()) {
                console.draw(pos, "[]", Color::DarkGray, Color::Black);
            }
        } else if (piece.hasPill() && piece.pillCellIndex() == i) {
            const Color pillColor = piece.pill() == PillType::Blue ? Color::Blue : Color::Red;
            console.draw(pos, "()", pillColor, Color::White);
        } else {
            console.draw(pos, "  ", piece.color(), piece.color());
        }
    }
}
