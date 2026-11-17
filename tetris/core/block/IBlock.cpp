#include "block/IBlock.h"

int IBlock::getBoxSize() const {
    return 4;
}

char IBlock::getSymbol() const {
    return 'I';
}

std::unique_ptr<Tetromino> IBlock::clone() const {
    return std::make_unique<IBlock>(*this);
}

std::vector<Point> IBlock::getBaseShape() const {
    return { Point(1, 0), Point(1, 1), Point(1, 2), Point(1, 3) };
}
