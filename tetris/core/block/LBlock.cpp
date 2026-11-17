#include "block/LBlock.h"

int LBlock::getBoxSize() const {
    return 3;
}

char LBlock::getSymbol() const {
    return 'L';
}

std::unique_ptr<Tetromino> LBlock::clone() const {
    return std::make_unique<LBlock>(*this);
}

std::vector<Point> LBlock::getBaseShape() const {
    return { Point(0, 2), Point(1, 0), Point(1, 1), Point(1, 2) };
}
