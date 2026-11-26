#include "block/Tetromino.h"
using namespace std;

namespace {

class ShapeData {
public:
    array<Point, Tetromino::kCellCount> cells;
    int boxSize;  // 회전 기준이 되는 정사각형 크기
};

// 회전 0 상태의 모양. 나머지 회전은 boxSize 안에서 시계 방향으로 계산한다.
const array<ShapeData, kTetrominoTypeCount> kShapes = {{
    {{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}}, 4},  // I
    {{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}, 2},  // O
    {{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}}, 3},  // T
    {{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}}, 3},  // J
    {{{{2, 0}, {0, 1}, {1, 1}, {2, 1}}}, 3},  // L
    {{{{1, 0}, {2, 0}, {0, 1}, {1, 1}}}, 3},  // S
    {{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}}, 3},  // Z
}};

Point rotateClockwise(Point p, int boxSize) {
    return {boxSize - 1 - p.y, p.x};
}

}  // namespace

Tetromino::Tetromino(TetrominoType type) : Tetromino(type, Point{0, 0}) {}

Tetromino::Tetromino(TetrominoType type, Point position) : m_type(type), m_position(position) {}

Tetromino::Cells Tetromino::localCells() const {
    const ShapeData& shape = kShapes[static_cast<int>(m_type)];
    Cells result = shape.cells;
    for (Point& p : result) {
        for (int i = 0; i < m_rotation; ++i) {
            p = rotateClockwise(p, shape.boxSize);
        }
    }
    return result;
}

Tetromino::Cells Tetromino::cells() const {
    Cells result = localCells();
    for (Point& p : result) {
        p += m_position;
    }
    return result;
}

Tetromino Tetromino::moved(Point delta) const {
    Tetromino copy = *this;
    copy.m_position += delta;
    return copy;
}

Tetromino Tetromino::rotated() const {
    return rotated(1);
}

Tetromino Tetromino::rotated(int times) const {
    Tetromino copy = *this;
    copy.m_rotation = ((m_rotation + times) % 4 + 4) % 4;
    return copy;
}

Tetromino Tetromino::morphed(TetrominoType type) const {
    Tetromino copy = *this;
    copy.m_type = type;
    return copy;
}

Tetromino Tetromino::withPill(PillType pill, int cellIndex) const {
    Tetromino copy = *this;
    copy.m_hasPill = true;
    copy.m_pill = pill;
    copy.m_pillIndex = cellIndex % kCellCount;
    return copy;
}

Color Tetromino::colorOf(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return Color::Cyan;
        case TetrominoType::O: return Color::Yellow;
        case TetrominoType::T: return Color::Magenta;
        case TetrominoType::J: return Color::Blue;
        case TetrominoType::L: return Color::DarkYellow;
        case TetrominoType::S: return Color::Green;
        case TetrominoType::Z: return Color::Red;
    }
    return Color::Gray;
}
