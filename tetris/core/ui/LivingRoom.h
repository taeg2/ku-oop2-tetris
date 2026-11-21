#pragma once
#include "ui/PixelArt.h"
// 할아버지 집 거실을 옆에서 본 그림. Stage 3 마지막 라운드와 오프닝이 같이 쓴다
class LivingRoom {
public:
    static void draw(Frame& f, int t, int sofaX, int doorX, bool tvStatic, int clockSpeed);
};
