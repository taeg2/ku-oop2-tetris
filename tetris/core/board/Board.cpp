#include "board/Board.h"

#include <algorithm>

Board::Board(int width, int height)
    : width(width), height(height), cells(height, std::vector<char>(width, emptySymbol)) {}

int Board::getWidth() const {
    return width;
}

int Board::getHeight() const {
    return height;
}

bool Board::isInside(const Point& cell) const {
    return cell.getRow() >= 0 && cell.getRow() < height
        && cell.getCol() >= 0 && cell.getCol() < width;
}

bool Board::isEmpty(const Point& cell) const {
    return isInside(cell) && cells[cell.getRow()][cell.getCol()] == emptySymbol;
}

char Board::getSymbolAt(const Point& cell) const {
    if (!isInside(cell)) {
        return emptySymbol;
    }
    return cells[cell.getRow()][cell.getCol()];
}

bool Board::canPlace(const std::vector<Point>& targetCells) const {
    return std::all_of(targetCells.begin(), targetCells.end(),
        [this](const Point& cell) { return isEmpty(cell); });
}

std::vector<int> Board::getFullLines() const {
    std::vector<int> fullLines;
    for (int row = 0; row < height; ++row) {
        if (isLineFull(row)) {
            fullLines.push_back(row);
        }
    }
    return fullLines;
}

std::vector<Point> Board::getFilledCells() const {
    std::vector<Point> filled;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            if (cells[row][col] != emptySymbol) {
                filled.push_back(Point(row, col));
            }
        }
    }
    return filled;
}

void Board::lockCells(const std::vector<Point>& targetCells, char symbol) {
    for (const auto& cell : targetCells) {
        if (isInside(cell)) {
            cells[cell.getRow()][cell.getCol()] = symbol;
        }
    }
}

void Board::clearLines(const std::vector<int>& rows) {
    // 지우지 않을 줄만 위에서부터 순서대로 남긴다
    std::vector<std::vector<char>> remaining;
    for (int row = 0; row < height; ++row) {
        if (std::find(rows.begin(), rows.end(), row) == rows.end()) {
            remaining.push_back(cells[row]);
        }
    }

    // 지운 줄 수만큼 맨 위에 빈 줄을 채운다
    const int clearedCount = height - static_cast<int>(remaining.size());
    remaining.insert(remaining.begin(), clearedCount, makeEmptyLine());
    cells = remaining;
}

void Board::clearBottomLine() {
    clearLines({ height - 1 });
}

void Board::clearCells(const std::vector<Point>& targetCells) {
    lockCells(targetCells, emptySymbol);
}

void Board::reset() {
    for (auto& line : cells) {
        line = makeEmptyLine();
    }
}

bool Board::isLineFull(int row) const {
    const auto& line = cells[row];
    return std::find(line.begin(), line.end(), emptySymbol) == line.end();
}

std::vector<char> Board::makeEmptyLine() const {
    return std::vector<char>(width, emptySymbol);
}
