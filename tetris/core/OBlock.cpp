#include "OBlock.h"

int OBlock::getBoxSize() const {
    return 2;
}

char OBlock::getSymbol() const {
    return 'O';
}

std::unique_ptr<Tetromino> OBlock::clone() const {
    return std::make_unique<OBlock>(*this);
}

std::vector<Point> OBlock::getBaseShape() const {
    return { Point(0, 0), Point(0, 1), Point(1, 0), Point(1, 1) };
}
