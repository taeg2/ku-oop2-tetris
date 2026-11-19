#pragma once
#include "ui/PixelArt.h"
// 집의 세 방(거실, 부엌, 서재)을 240x135로 그리고 zoomOut으로 화면 크기에 맞춘다
class HouseRooms {
public:
    enum Room { LIVING, KITCHEN, LIBRARY };
    static const int W = 240, H = 135, ZOOM = 2;
    static void draw(Frame& world, Room room, int t, bool albumOnChair = true, bool candyTaken = false, int stage = 0);
    static void zoomOut(Frame& out, const Frame& world);
private:
    static void living(Frame& f, int t);
    static void kitchen(Frame& f, int t, bool candyTaken, int stage);
    static void library(Frame& f, int t, bool albumOnChair);
};
