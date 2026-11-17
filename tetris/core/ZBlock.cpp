#include "ZBlock.h"

int ZBlock::getBoxSize() const {
    return 3;
}

char ZBlock::getSymbol() const {
    return 'Z';
}

std::unique_ptr<Tetromino> ZBlock::clone() const {
    return std::make_unique<ZBlock>(*this);
}

std::vector<Point> ZBlock::getBaseShape() const {
    return { Point(0, 0), Point(0, 1), Point(1, 1), Point(1, 2) };
}
