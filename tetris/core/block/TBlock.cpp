#include "block/TBlock.h"

int TBlock::getBoxSize() const {
    return 3;
}

char TBlock::getSymbol() const {
    return 'T';
}

std::unique_ptr<Tetromino> TBlock::clone() const {
    return std::make_unique<TBlock>(*this);
}

std::vector<Point> TBlock::getBaseShape() const {
    return { Point(0, 1), Point(1, 0), Point(1, 1), Point(1, 2) };
}
