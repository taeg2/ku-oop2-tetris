#pragma once
#include "common/Point.h"

// 직사각형 영역 (왼쪽 위 좌표 + 크기)
struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    int right() const { return x + width; }
    int bottom() const { return y + height; }
    bool contains(Point p) const {
        return p.x >= x && p.x < right() && p.y >= y && p.y < bottom();
    }
};
