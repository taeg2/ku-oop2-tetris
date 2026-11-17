#include "block/SBlock.h"

int SBlock::getBoxSize() const {
    return 3;
}

char SBlock::getSymbol() const {
    return 'S';
}

std::unique_ptr<Tetromino> SBlock::clone() const {
    return std::make_unique<SBlock>(*this);
}

std::vector<Point> SBlock::getBaseShape() const {
    return { Point(0, 1), Point(0, 2), Point(1, 0), Point(1, 1) };
}
