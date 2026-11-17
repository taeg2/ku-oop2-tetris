#include "JBlock.h"

int JBlock::getBoxSize() const {
    return 3;
}

char JBlock::getSymbol() const {
    return 'J';
}

std::unique_ptr<Tetromino> JBlock::clone() const {
    return std::make_unique<JBlock>(*this);
}

std::vector<Point> JBlock::getBaseShape() const {
    return { Point(0, 0), Point(1, 0), Point(1, 1), Point(1, 2) };
}
