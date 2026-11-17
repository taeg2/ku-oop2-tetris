#pragma once

#include "Tetromino.h"

// S 블록. 회전 상자 3x3 안에서 처음 모양은 다음과 같다.
//   .SS
//   SS.
//   ...
class SBlock : public Tetromino {
public:
    int getBoxSize() const override;
    char getSymbol() const override;
    std::unique_ptr<Tetromino> clone() const override;

protected:
    std::vector<Point> getBaseShape() const override;
};
