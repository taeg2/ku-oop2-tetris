#include "board/Board.h"
#include <algorithm>
#include <stdexcept>
#include <string>
using namespace std;

Board::Board() {
    reset();
}

bool Board::isInside(Point pos) const {
    return pos.x >= 0 && pos.x < kWidth && pos.y >= 0 && pos.y < kHeight;
}

const Cell& Board::cellAt(Point pos) const {
    if (!isInside(pos)) {
        throw out_of_range("Board::cellAt(" + to_string(pos.x) + ", " + to_string(pos.y) + ")");
    }
    return m_grid[pos.y][pos.x];
}

Cell& Board::at(Point pos) {
    if (!isInside(pos)) {
        throw out_of_range("Board::cellAt(" + to_string(pos.x) + ", " + to_string(pos.y) + ")");
    }
    return m_grid[pos.y][pos.x];
}

bool Board::canPlace(const Tetromino& piece) const {
    return canPlace(piece, Point{0, 0});
}

bool Board::canPlace(const Tetromino& piece, Point offset) const {
    for (Point p : piece.cells()) {
        p += offset;
        if (p.x < 0 || p.x >= kWidth || p.y >= kHeight) {
            return false;
        }
        if (p.y >= 0 && m_grid[p.y][p.x].isFilled()) {  // 보드 위쪽(y < 0)은 비어 있는 것으로 본다
            return false;
        }
    }
    return true;
}

bool Board::canLock(const Tetromino& piece) const {
    for (Point p : piece.cells()) {
        if (!isInside(p)) {
            return false;
        }
    }
    return canPlace(piece);
}

void Board::lock(const Tetromino& piece) {
    const Tetromino::Cells cells = piece.cells();
    for (int i = 0; i < Tetromino::kCellCount; ++i) {
        const Point p = cells[i];
        if (!isInside(p)) {
            continue;  // 보드 밖 칸은 버린다 (canLock()으로 먼저 확인할 것)
        }
        if (piece.hasPill() && piece.pillCellIndex() == i) {
            at(p).fill(piece.color(), piece.pill());
        } else {
            at(p).fill(piece.color());
        }
    }
}

vector<int> Board::findFullLines() const {
    vector<int> rows;
    for (int y = 0; y < kHeight; ++y) {
        const auto& row = m_grid[y];
        if (all_of(row.begin(), row.end(), [](const Cell& c) { return c.isFilled(); })) {
            rows.push_back(y);
        }
    }
    return rows;
}

vector<PillType> Board::pillsInRows(const vector<int>& rows) const {
    vector<PillType> pills;
    for (int y : rows) {
        for (int x = 0; x < kWidth; ++x) {
            const Cell& cell = cellAt({x, y});
            if (cell.hasPill()) {
                pills.push_back(cell.pill());
            }
        }
    }
    return pills;
}

void Board::clearRows(const vector<int>& rows) {
    for (int y : rows) {
        if (y < 0 || y >= kHeight) {
            throw out_of_range("Board::clearRows row " + to_string(y));
        }
    }

    // 지우지 않을 줄만 아래에서부터 다시 쌓는다.
    array<array<Cell, kWidth>, kHeight> next{};
    int target = kHeight - 1;
    for (int y = kHeight - 1; y >= 0; --y) {
        if (find(rows.begin(), rows.end(), y) == rows.end()) {
            next[target--] = m_grid[y];
        }
    }
    m_grid = next;
}

void Board::clearRow(int y) {
    clearRows({y});
}

void Board::clearRow() {
    clearRow(kHeight - 1);
}

bool Board::isRowEmpty(int y) const {
    const auto& row = m_grid.at(static_cast<size_t>(y));
    return none_of(row.begin(), row.end(), [](const Cell& c) { return c.isFilled(); });
}

void Board::removeCell(Point pos) {
    if (isInside(pos)) {
        at(pos).clear();
    }
}

void Board::removeCells(const vector<Point>& cells) {
    for (Point p : cells) {
        removeCell(p);
    }
}

void Board::setHidden(const Rect& area, bool hidden) {
    for (int y = area.y; y < area.bottom(); ++y) {
        for (int x = area.x; x < area.right(); ++x) {
            if (isInside({x, y})) {
                m_hidden[y][x] = hidden;
            }
        }
    }
}

void Board::setHidden(bool hidden) {
    setHidden(Rect{0, 0, kWidth, kHeight}, hidden);
}

bool Board::isHidden(Point pos) const {
    return isInside(pos) && m_hidden[pos.y][pos.x];
}

void Board::setMarked(const vector<Point>& cells, bool marked) {
    for (Point p : cells) {
        if (isInside(p)) {
            m_marked[p.y][p.x] = marked;
        }
    }
}

void Board::setMarked(bool marked) {
    for (auto& row : m_marked) {
        row.fill(marked);
    }
}

bool Board::isMarked(Point pos) const {
    return isInside(pos) && m_marked[pos.y][pos.x];
}

vector<Point> Board::filledCells() const {
    vector<Point> cells;
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            if (m_grid[y][x].isFilled()) {
                cells.push_back({x, y});
            }
        }
    }
    return cells;
}

void Board::reset() {
    for (auto& row : m_grid) {
        for (Cell& cell : row) {
            cell.clear();
        }
    }
    setHidden(false);
    setMarked(false);
}
