#pragma once

#include "block/Tetromino.h"

// 네모(O) 블록. 회전 상자 2x2 안에서 처음 모양은 다음과 같다.
//   OO
//   OO
class OBlock : public Tetromino {
public:
    int getBoxSize() const override;
    char getSymbol() const override;
    std::unique_ptr<Tetromino> clone() const override;

protected:
    std::vector<Point> getBaseShape() const override;
};
