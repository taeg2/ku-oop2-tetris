#include "block/Tetromino.h"

namespace {
    const int rotationStateCount = 4;
}

Tetromino::Tetromino() : position(), rotation(0) {}

std::vector<Point> Tetromino::getCells() const {
    return toBoardCells(getShape(rotation), position);
}

std::vector<Point> Tetromino::getCellsAfterMove(int deltaRow, int deltaCol) const {
    return toBoardCells(getShape(rotation), position.translated(deltaRow, deltaCol));
}

std::vector<Point> Tetromino::getCellsAfterRotate() const {
    return toBoardCells(getShape((rotation + 1) % rotationStateCount), position);
}

Point Tetromino::getPosition() const {
    return position;
}

int Tetromino::getRotation() const {
    return rotation;
}

void Tetromino::moveBy(int deltaRow, int deltaCol) {
    position = position.translated(deltaRow, deltaCol);
}

void Tetromino::moveTo(const Point& newPosition) {
    position = newPosition;
}

void Tetromino::rotateClockwise() {
    rotation = (rotation + 1) % rotationStateCount;
}

std::vector<Point> Tetromino::getShape(int rotationCount) const {
    std::vector<Point> shape = getBaseShape();
    const int boxSize = getBoxSize();

    // 정사각형 상자 안에서 시계 방향 90도 회전: (r, c) -> (c, boxSize - 1 - r)
    for (int i = 0; i < rotationCount; ++i) {
        for (auto& cell : shape) {
            cell = Point(cell.getCol(), boxSize - 1 - cell.getRow());
        }
    }
    return shape;
}

std::vector<Point> Tetromino::toBoardCells(const std::vector<Point>& shape, const Point& origin) const {
    std::vector<Point> cells;
    for (const auto& cell : shape) {
        cells.push_back(origin.translated(cell.getRow(), cell.getCol()));
    }
    return cells;
}
