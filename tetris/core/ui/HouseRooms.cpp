#include "ui/HouseRooms.h"
#include <cmath>
namespace {
const RGB OL{ 58, 32, 28 };
uint8_t ol() { return C(OL); }
RGB dark(RGB c, float k) { return mix(c, RGB{ 40, 20, 24 }, k); }
RGB lite(RGB c, float k) { return mix(c, RGB{ 255, 246, 220 }, k); }
void box(Frame& f, int x, int y, int w, int h, RGB c) { f.rect(x, y, w, h, ol()); f.rect(x + 1, y + 1, w - 2, h - 2, C(c)); f.hline(x + 1, x + w - 2, y + 1, C(lite(c, 0.25f))); }
void planks(Frame& f, int x, int y, int w, int h, RGB a, int len) {
    RGB b = dark(a, 0.08f), seam = dark(a, 0.35f);
    for (int j = 0; j < h; j += 7) {
        int row = j / 7;
        f.rect(x, y + j, w, std::min(7, h - j), C(row % 2 ? a : b));
        f.hline(x, x + w - 1, y + j, C(seam));
        int off = (row * 17) % len;
        for (int i = x - off; i < x + w; i += len) { if (i < x) continue; f.vline(i, y + j, y + std::min(j + 6, h - 1), C(seam)); f.hline(i + 3, i + 6, y + j + 3 + row % 2, C(dark(a, 0.15f))); }
    }
}
void wall(Frame& f, int y1, int panelTop, RGB paper, RGB panel) {
    f.rect(0, 0, HouseRooms::W, y1, C(paper));
    f.rect(0, panelTop, HouseRooms::W, y1 - panelTop, C(panel));
    f.hline(0, HouseRooms::W - 1, panelTop, ol()); f.hline(0, HouseRooms::W - 1, panelTop + 1, C(lite(panel, 0.4f)));
    for (int x = 4; x < HouseRooms::W; x += 16) { f.rect(x, panelTop + 4, 12, y1 - panelTop - 8, C(dark(panel, 0.08f))); f.rect(x + 1, panelTop + 5, 10, y1 - panelTop - 10, C(panel)); }
    f.rect(0, y1 - 3, HouseRooms::W, 3, C(dark(panel, 0.35f))); f.hline(0, HouseRooms::W - 1, y1, ol());
}
void books(Frame& f, int x, int y, int w, int h, int seed) {
    static const RGB B[7] = { {200, 70, 66}, {66, 120, 190}, {80, 160, 90}, {226, 180, 70}, {150, 90, 170}, {236, 140, 70}, {60, 150, 150} };
    for (int i = 0, k = seed; i < w; ++k) {
        int bw = 2 + k % 2, bh = h - (k % 3);
        if (i + bw > w) break;
        RGB c = B[(k * 5 + seed) % 7];
        f.rect(x + i, y + h - bh, bw, bh, C(c)); f.set(x + i, y + h - bh, C(lite(c, 0.4f))); f.vline(x + i + bw - 1, y + h - bh, y + h - 1, C(dark(c, 0.3f)));
        if (k % 4 == 0) f.hline(x + i, x + i + bw - 1, y + h - bh + 2, C(250, 230, 160));
        i += bw + (k % 6 == 0 ? 2 : 0);
    }
}
void bookcase(Frame& f, int x, int y, int w, int h, RGB wood, int rows, int seed) {
    f.rect(x + 2, y + h, w - 2, 2, C(dark(wood, 0.6f)));
    box(f, x, y, w, h, wood);
    int rh = (h - 4) / rows;
    for (int r = 0; r < rows; ++r) {
        int ry = y + 2 + r * rh;
        f.rect(x + 2, ry, w - 4, rh - 2, C(dark(wood, 0.45f)));
        books(f, x + 3, ry + 1, w - 6, rh - 3, seed + r * 3);
        f.hline(x + 1, x + w - 2, ry + rh - 2, C(lite(wood, 0.15f))); f.hline(x + 1, x + w - 2, ry + rh - 1, ol());
    }
}
void plant(Frame& f, int x, int y, int r) {
    box(f, x - 5, y, 10, 8, { 190, 96, 70 }); f.hline(x - 5, x + 4, y + 2, C(150, 70, 52));
    f.disc(x, y - r + 1, r + 1, ol()); f.disc(x, y - r + 1, r, C(70, 140, 80));
    for (int k = 0; k < 6; ++k) { int dx = (k * 5) % (2 * r) - r + 2, dy = (k * 3) % (2 * r) - r; f.disc(x + dx / 2, y - r + 1 + dy / 2, 2, C(k % 2 ? RGB{ 110, 180, 100 } : RGB{ 50, 110, 64 })); }
}
void clock(Frame& f, int x, int y, int t) {
    f.disc(x, y, 7, ol()); f.disc(x, y, 6, C(250, 248, 240)); f.disc(x, y, 1, ol());
    float a = t * 0.02f - 1.57f; f.line(x, y, x + (int)std::lround(std::cos(a) * 4), y + (int)std::lround(std::sin(a) * 4), ol()); f.line(x, y, x, y - 3, ol());
}
void candle(Frame& f, int x, int y, int t) {
    int fl = (t / 2) % 3;
    f.rect(x - 1, y - 5, 3, 5, C(250, 244, 226)); f.set(x + 1, y - 5, C(220, 210, 190));
    f.disc(x, y - 7 - (fl == 1), 1, C(250, 170, 60)); f.set(x, y - 8 - fl % 2, C(255, 240, 150));
}
void stool(Frame& f, int x, int y) { f.rect(x + 1, y + 9, 12, 2, C(120, 70, 50)); box(f, x, y, 14, 10, { 130, 160, 120 }); f.hline(x + 2, x + 11, y + 6, C(110, 140, 100)); f.set(x + 6, y + 4, C(100, 130, 92)); }
}
void HouseRooms::zoomOut(Frame& out, const Frame& world) {
    for (int y = 0; y < Frame::H; ++y) for (int x = 0; x < Frame::W; ++x) out.set(x, y, world.get(x / ZOOM, y / ZOOM));
}
void HouseRooms::draw(Frame& world, Room room, int t, bool albumOnChair, bool candyTaken, int stage) {
    world.rect(0, 0, W, H, C(30, 22, 30));
    if (room == LIVING) living(world, t);
    else if (room == KITCHEN) kitchen(world, t, candyTaken, stage);
    else library(world, t, albumOnChair);
}
void HouseRooms::living(Frame& f, int t) {
    wall(f, 60, 36, { 244, 228, 174 }, { 230, 226, 220 });
    planks(f, 0, 61, W, H - 61, { 214, 150, 84 }, 34);
    bookcase(f, 32, 24, 36, 40, { 236, 230, 220 }, 4, 3);
    f.rect(38, 10, 26, 13, ol()); f.rect(39, 11, 24, 11, C(250, 180, 90)); f.rect(39, 17, 24, 5, C(70, 120, 170)); f.disc(51, 16, 3, C(255, 236, 140)); f.hline(39, 62, 17, C(200, 120, 70));
    plant(f, 76, 56, 7);
    f.rect(6, 22, 22, 39, ol()); f.rect(7, 23, 20, 38, C(150, 96, 60)); f.rect(9, 25, 16, 15, C(126, 78, 48)); f.rect(9, 43, 16, 15, C(126, 78, 48)); f.rect(23, 42, 2, 3, C(232, 200, 110));
    f.rect(4, 60, 26, 2, C(110, 70, 44));
    box(f, 88, 36, 4, 24, { 196, 196, 204 }); box(f, 150, 36, 4, 24, { 196, 196, 204 });
    box(f, 96, 14, 48, 26, { 40, 44, 60 }); f.rect(98, 16, 44, 22, C(50, 120, 170)); f.line(100, 18, 112, 30, C(110, 170, 210)); f.rect(100, 32, 14, 3, C(220, 120, 120));
    box(f, 98, 44, 44, 15, { 236, 230, 220 }); f.rect(104, 49, 32, 6, C(200, 196, 190)); f.rect(116, 51, 8, 2, ol()); f.rect(100, 59, 2, 3, ol()); f.rect(138, 59, 2, 3, ol());
    box(f, 110, 40, 20, 5, { 160, 160, 170 });
    clock(f, 168, 14, t);
    f.rect(190, 0, 6, 38, ol()); f.rect(191, 0, 4, 38, C(110, 110, 116));
    f.disc(193, 46, 16, ol()); f.disc(193, 46, 15, C(206, 200, 196)); f.rect(176, 46, 34, 16, ol()); f.rect(177, 46, 32, 15, C(206, 200, 196));
    for (int k = 0; k < 6; ++k) { float a = 3.1416f + k * 0.63f; f.set(193 + (int)(std::cos(a) * 12), 46 + (int)(std::sin(a) * 12), C(170, 164, 160)); }
    f.disc(193, 50, 9, ol()); f.rect(184, 50, 19, 9, ol()); f.disc(193, 50, 8, C(50, 30, 30)); f.rect(185, 50, 17, 8, C(50, 30, 30));
    int fl = (t / 2) % 3;
    f.poly({ {187, 58}, {199, 58}, {196, 50 - fl}, {193, 45 + fl}, {190, 51} }, C(240, 140, 50)); f.poly({ {190, 58}, {196, 58}, {194, 52 - fl}, {192, 51} }, C(255, 220, 110));
    f.rect(184, 57, 19, 2, C(90, 60, 40));
    f.ellipse(193, 78, 14, 6, C(dark({ 214, 150, 84 }, 0.2f))); f.ellipse(193, 77, 14, 6, C(222, 214, 204)); f.ellipse(193, 77, 9, 4, C(232, 226, 218));
    f.rect(214, 18, 22, 43, ol()); f.rect(216, 20, 18, 41, C(14, 10, 14)); f.rect(214, 18, 22, 2, C(150, 140, 136)); f.vline(214, 18, 60, C(150, 140, 136)); f.vline(235, 18, 60, C(150, 140, 136));
    f.ellipse(120, 92, 30, 12, C(dark({ 214, 150, 84 }, 0.2f))); f.ellipse(120, 91, 30, 12, C(222, 214, 204)); f.ellipse(120, 91, 22, 8, C(232, 226, 218));
    f.rect(106, 94, 2, 6, ol()); f.rect(132, 94, 2, 6, ol());
    f.ellipse(120, 90, 16, 5, ol()); f.ellipse(120, 89, 15, 4, C(244, 240, 232));
    f.rect(119, 80, 2, 8, C(60, 150, 170)); f.disc(120, 86, 2, C(60, 150, 170));
    for (int k = 0; k < 5; ++k) { int fx = 114 + k * 3, fy = 74 + (k % 2) * 3; f.line(120, 82, fx, fy + 2, C(70, 130, 70)); f.set(fx, fy, C(250, 250, 244)); f.set(fx + 1, fy, C(250, 250, 244)); f.set(fx, fy + 1, C(250, 230, 120)); }
    auto armchair = [&](int x, bool faceRight) {
        RGB c{ 232, 200, 172 };
        f.rect(x + 2, 102, 22, 3, C(dark({ 214, 150, 84 }, 0.4f)));
        box(f, x, 66, 24, 36, c);
        int bx = faceRight ? x : x + 18;
        box(f, bx, 64, 6, 38, dark(c, 0.06f));
        box(f, x + (faceRight ? 5 : 1), 64, 18, 8, dark(c, 0.04f));
        box(f, x + (faceRight ? 5 : 1), 94, 18, 8, dark(c, 0.04f));
        f.rect(x + (faceRight ? 7 : 3), 74, 14, 18, C(lite(c, 0.15f)));
        f.rect(x + 3, 102, 3, 3, ol()); f.rect(x + 18, 102, 3, 3, ol());
        if (faceRight) { f.disc(x + 10, 84, 4, ol()); f.disc(x + 10, 84, 3, C(244, 230, 214)); } else { f.disc(x + 13, 80, 3, ol()); f.disc(x + 13, 80, 2, C(220, 150, 150)); }
    };
    armchair(56, true); armchair(160, false);
    RGB s{ 226, 192, 164 };
    f.rect(100, 130, 42, 3, C(dark({ 214, 150, 84 }, 0.4f)));
    box(f, 98, 108, 46, 22, s); box(f, 96, 112, 8, 18, dark(s, 0.06f)); box(f, 138, 112, 8, 18, dark(s, 0.06f));
    f.vline(121, 110, 128, C(dark(s, 0.2f))); f.hline(100, 142, 112, C(lite(s, 0.2f)));
    f.rect(212, 128, 14, 3, C(dark({ 214, 150, 84 }, 0.4f)));
    f.line(219, 106, 213, 130, ol()); f.line(219, 106, 225, 130, ol()); f.line(219, 106, 219, 130, ol());
    f.vline(219, 92, 106, ol()); f.vline(220, 92, 106, C(120, 80, 50));
    box(f, 212, 82, 16, 11, { 244, 228, 170 }); for (int k = 0; k < 7; ++k) f.vline(214 + k * 2, 88, 91, C(220, 196, 140));
    f.rect(0, 96, 4, 24, C(dark({ 214, 150, 84 }, 0.3f)));
    for (int k = 0; k < 3; ++k) { int ax = 6 + k * 4; f.set(ax, 108, C(250, 236, 200)); }
    f.rect(0, 0, W, 1, ol()); f.rect(0, H - 1, W, 1, ol()); f.rect(0, 0, 1, H, ol()); f.rect(W - 1, 0, 1, H, ol());
}
void HouseRooms::kitchen(Frame& f, int t, bool candyTaken, int stage) {
    wall(f, 62, 36, { 242, 230, 172 }, { 228, 226, 222 });
    planks(f, 0, 63, W, H - 63, { 132, 82, 52 }, 30);
    clock(f, 30, 14, t);
    f.rect(44, 16, 34, 20, ol()); f.rect(45, 17, 32, 18, C(150, 98, 60)); f.rect(47, 19, 28, 14, C(170, 214, 236));
    f.rect(47, 27, 28, 6, C(90, 150, 200)); f.poly({ {47, 28}, {58, 21}, {66, 28} }, C(70, 130, 80)); f.rect(62, 24, 6, 5, C(150, 90, 60)); f.poly({ {61, 24}, {65, 20}, {69, 24} }, C(190, 70, 60)); f.disc(71, 21, 2, C(255, 236, 140));
    f.rect(92, 64, 22, 3, C(70, 40, 30));
    box(f, 90, 26, 24, 40, { 230, 230, 234 }); f.hline(91, 112, 42, ol()); f.rect(108, 30, 2, 8, C(160, 160, 170)); f.rect(108, 46, 2, 10, C(160, 160, 170));
    f.rect(94, 30, 8, 8, C(250, 248, 236)); f.rect(95, 31, 6, 3, C(120, 180, 230)); f.rect(95, 34, 6, 3, C(110, 190, 90)); f.disc(97, 33, 1, C(240, 200, 80)); f.disc(104, 48, 2, C(220, 80, 80)); f.disc(98, 52, 2, C(80, 160, 220));
    auto window = [&](int x) {
        f.rect(x, 6, 34, 26, ol()); f.rect(x + 1, 7, 32, 24, C(150, 96, 60));
        for (int k = 0; k < 2; ++k) { f.rect(x + 3 + k * 15, 9, 13, 20, C(176, 218, 240)); f.line(x + 5 + k * 15, 26, x + 13 + k * 15, 12, C(214, 238, 250)); }
        f.rect(x - 1, 31, 36, 2, C(120, 76, 48));
    };
    window(122); window(168);
    f.rect(132, 22, 5, 9, ol()); f.rect(133, 23, 3, 8, C(170, 40, 60)); for (int k = 0; k < 4; ++k) f.disc(130 + k * 3, 19 - (k % 2) * 2, 2, C(250, 230, 236));
    box(f, 118, 36, 92, 9, { 120, 76, 48 });
    f.rect(122, 37, 26, 6, ol()); f.rect(123, 38, 24, 4, C(150, 150, 160)); f.rect(134, 30, 2, 8, C(150, 150, 160)); f.rect(134, 30, 5, 2, C(150, 150, 160));
    f.rect(152, 37, 24, 6, C(80, 60, 66)); for (int k = 0; k < 2; ++k) for (int j = 0; j < 2; ++j) f.rect(155 + k * 11, 38 + j * 2, 7, 1, C(40, 30, 34));
    f.rect(180, 28, 12, 9, ol()); f.rect(181, 29, 10, 8, C(220, 236, 244)); f.rect(180, 26, 12, 3, ol()); f.rect(181, 27, 10, 1, C(200, 90, 90));
    for (int k = 0; k < (stage > 0 || candyTaken ? 7 : 8); ++k) f.rect(182 + (k % 4) * 2, 31 + (k / 4) * 3, 2, 2, C(k % 3 == 0 ? RGB{ 240, 90, 120 } : k % 3 == 1 ? RGB{ 250, 210, 70 } : RGB{ 90, 190, 230 }));
    box(f, 118, 45, 32, 22, { 168, 168, 96 }); f.vline(134, 46, 65, ol()); f.rect(130, 54, 2, 2, ol()); f.rect(137, 54, 2, 2, ol());
    box(f, 150, 45, 28, 22, { 100, 60, 64 }); f.rect(152, 47, 24, 3, C(60, 40, 44)); for (int k = 0; k < 5; ++k) f.set(154 + k * 5, 48, C(230, 120, 60)); f.rect(153, 52, 22, 12, C(50, 32, 36));
    box(f, 178, 45, 32, 22, { 168, 168, 96 }); for (int k = 0; k < 3; ++k) { f.hline(179, 208, 51 + k * 6, ol()); f.rect(193, 48 + k * 6, 2, 1, ol()); }
    f.rect(210, 36, 16, 50, ol()); f.rect(211, 37, 14, 30, C(120, 76, 48)); f.rect(211, 67, 14, 18, C(168, 168, 96)); f.hline(211, 224, 67, ol());
    f.rect(216, 26, 4, 10, C(110, 180, 190)); f.line(218, 26, 214, 18, C(70, 130, 70)); f.line(218, 26, 222, 19, C(70, 130, 70)); f.set(214, 17, C(250, 250, 240)); f.set(222, 18, C(250, 250, 240)); f.set(218, 16, C(250, 250, 240));
    f.rect(36, 66, 40, 66, C(120, 40, 44)); for (int k = 0; k < 12; ++k) f.set(38 + k * 3, 130, C(200, 160, 120));
    auto chairTD = [&](int x, int y, bool back) { if (back) { box(f, x, y, 14, 6, { 140, 90, 56 }); } box(f, x, y + (back ? 5 : 0), 14, 10, { 120, 76, 48 }); f.hline(x + 2, x + 11, y + (back ? 8 : 3), C(150, 98, 60)); };
    chairTD(49, 58, true);
    chairTD(26, 80, false); chairTD(26, 98, false); chairTD(72, 80, false); chairTD(72, 98, false);
    f.rect(44, 112, 26, 3, C(70, 40, 30));
    box(f, 42, 72, 30, 42, { 130, 80, 48 }); for (int k = 0; k < 6; ++k) f.vline(46 + k * 4, 76, 110, C(116, 70, 42)); f.hline(43, 70, 73, C(156, 102, 64));
    chairTD(49, 116, false);
    if (stage > 0) {
        f.ellipse(57, 92, 10, 5, ol()); f.ellipse(57, 91, 9, 4, C(250, 250, 246)); f.ellipse(57, 91, 6, 2, C(226, 226, 220));
        if (!candyTaken) { f.rect(50, 86, 14, 7, ol()); f.rect(51, 87, 12, 2, C(236, 196, 130)); f.rect(51, 89, 12, 1, C(110, 180, 80)); f.rect(51, 90, 12, 1, C(220, 80, 70)); f.rect(51, 91, 12, 1, C(236, 196, 130)); f.set(56, 85, C(236, 196, 130)); }
        else { f.set(54, 90, C(220, 180, 120)); f.set(59, 92, C(220, 180, 120)); }
    }
    f.rect(120, 80, 54, 10, C(226, 220, 210)); for (int k = 0; k < 6; ++k) f.vline(124 + k * 9, 80, 89, C(206, 200, 190)); for (int k = 0; k < 14; ++k) { f.set(119, 81 + (k % 9), C(200, 196, 186)); f.set(174, 81 + (k % 9), C(200, 196, 186)); }
    plant(f, 10, 86, 8); plant(f, 228, 126, 9);
    f.rect(W - 4, 96, 4, 24, C(70, 40, 30));
    f.rect(0, 0, W, 1, ol()); f.rect(0, H - 1, W, 1, ol()); f.rect(0, 0, 1, H, ol()); f.rect(W - 1, 0, 1, H, ol());
}
void HouseRooms::library(Frame& f, int t, bool albumOnChair) {
    RGB rim{ 236, 168, 76 }, rimD{ 196, 112, 56 };
    f.rect(0, 0, W, H, C(44, 30, 40));
    f.rect(8, 6, 224, 129, ol()); f.rect(9, 7, 222, 128, C(rimD)); f.rect(11, 9, 218, 126, C(rim)); f.rect(14, 12, 212, 123, C(rimD));
    planks(f, 16, 14, 208, 121, { 222, 140, 72 }, 22);
    f.rect(16, 14, 208, 8, C(90, 128, 100)); f.hline(16, 223, 22, ol());
    f.rect(104, 128, 32, 7, C(222, 140, 72)); f.rect(102, 128, 2, 7, C(rimD)); f.rect(136, 128, 2, 7, C(rimD));
    bookcase(f, 18, 16, 62, 40, { 196, 112, 56 }, 4, 1);
    f.rect(84, 26, 56, 26, ol()); f.rect(85, 27, 54, 24, C(236, 168, 76)); books(f, 88, 30, 26, 9, 5); f.hline(85, 138, 40, ol());
    books(f, 88, 42, 24, 8, 9);
    f.disc(126, 44, 6, ol()); f.disc(126, 44, 5, C(80, 150, 200)); f.disc(124, 43, 2, C(110, 190, 110)); f.rect(125, 50, 3, 2, C(200, 160, 60));
    books(f, 120, 30, 16, 9, 2);
    bookcase(f, 144, 16, 32, 40, { 196, 112, 56 }, 4, 7);
    f.vline(179, 18, 58, ol()); f.vline(187, 18, 58, ol()); f.vline(180, 18, 58, C(170, 100, 50)); f.vline(186, 18, 58, C(170, 100, 50)); for (int k = 0; k < 6; ++k) f.hline(180, 186, 22 + k * 7, C(150, 90, 46));
    bookcase(f, 190, 16, 32, 40, { 196, 112, 56 }, 4, 11);
    auto rugP = [&](int x, int y, int w, int h) {
        f.rect(x, y, w, h, ol()); f.rect(x + 1, y + 1, w - 2, h - 2, C(120, 80, 96));
        f.rect(x + 3, y + 3, w - 6, h - 6, C(150, 100, 110)); f.rect(x + 5, y + 5, w - 10, h - 10, C(120, 80, 96));
        for (int i = x + 4; i < x + w - 4; i += 3) { f.set(i, y + 2, C(190, 140, 130)); f.set(i, y + h - 3, C(190, 140, 130)); }
        for (int j = y + 4; j < y + h - 4; j += 3) { f.set(x + 2, j, C(190, 140, 130)); f.set(x + w - 3, j, C(190, 140, 130)); }
        for (int j = y + 8; j < y + h - 8; j += 6) for (int i = x + 8; i < x + w - 8; i += 6) { f.set(i, j, C(170, 120, 120)); f.set(i + 1, j + 1, C(170, 120, 120)); }
    };
    rugP(36, 62, 70, 40); rugP(150, 66, 48, 58);
    auto roundTable = [&](int cx, int cy, int r) { f.ellipse(cx + 1, cy + r / 2 + 3, r, r / 2, C(dark({ 222, 140, 72 }, 0.5f))); f.rect(cx - r + 3, cy + 2, 3, r / 2 + 3, ol()); f.rect(cx + r - 5, cy + 2, 3, r / 2 + 3, ol()); f.ellipse(cx, cy, r + 1, r * 3 / 4 + 1, ol()); f.ellipse(cx, cy, r, r * 3 / 4, C(236, 176, 84)); f.ellipse(cx - 2, cy - 2, r - 4, r * 3 / 4 - 4, C(244, 192, 100)); };
    roundTable(70, 78, 16);
    box(f, 58, 70, 12, 8, { 66, 100, 160 }); box(f, 58, 74, 12, 6, { 60, 150, 150 });
    box(f, 70, 68, 14, 9, { 244, 238, 222 }); f.vline(77, 69, 76, C(200, 190, 170)); for (int k = 0; k < 3; ++k) { f.hline(72, 75, 70 + k * 2, C(150, 140, 130)); f.hline(79, 82, 70 + k * 2, C(150, 140, 130)); }
    box(f, 72, 80, 10, 8, { 200, 60, 56 });
    stool(f, 38, 78); stool(f, 92, 66); stool(f, 64, 98);
    roundTable(174, 92, 14);
    box(f, 164, 86, 9, 11, { 244, 238, 222 }); f.vline(168, 87, 96, C(200, 190, 170));
    box(f, 178, 86, 10, 8, { 60, 150, 150 }); box(f, 178, 90, 10, 6, { 66, 100, 160 });
    candle(f, 175, 86, t);
    stool(f, 167, 68); stool(f, 167, 112);
    auto stand = [&](int x, int y) { f.ellipse(x, y + 18, 5, 2, ol()); f.ellipse(x, y + 17, 4, 1, C(200, 80, 56)); f.rect(x - 2, y, 5, 17, ol()); f.rect(x - 1, y + 1, 3, 16, C(210, 90, 60)); f.vline(x - 1, y + 1, y + 16, C(236, 130, 90)); candle(f, x, y, t); };
    stand(26, 66); stand(214, 62);
    auto desk = [&](int x, int y, int w) { f.rect(x + 2, y + 12, 3, 4, ol()); f.rect(x + w - 5, y + 12, 3, 4, ol()); box(f, x, y, w, 12, { 236, 176, 84 }); f.rect(x, y + 10, w, 2, C(196, 128, 60)); };
    desk(18, 110, 46); desk(176, 110, 46);
    box(f, 28, 104, 18, 9, { 244, 238, 222 }); f.vline(37, 105, 112, C(200, 190, 170)); candle(f, 54, 110, t);
    box(f, 184, 104, 18, 9, { 244, 238, 222 }); f.vline(193, 105, 112, C(200, 190, 170)); f.line(206, 110, 214, 104, C(250, 250, 244)); f.line(207, 110, 215, 105, C(200, 60, 56));
    int ax = 96, ay = 92;
    f.rect(ax + 1, ay + 18, 15, 2, C(dark({ 222, 140, 72 }, 0.5f)));
    box(f, ax, ay - 12, 16, 13, { 150, 86, 50 }); f.vline(ax + 5, ay - 10, ay - 2, C(120, 66, 40)); f.vline(ax + 10, ay - 10, ay - 2, C(120, 66, 40));
    box(f, ax, ay, 16, 12, { 170, 100, 58 }); f.rect(ax + 1, ay + 12, 14, 3, ol()); f.rect(ax + 1, ay + 15, 2, 4, ol()); f.rect(ax + 13, ay + 15, 2, 4, ol());
    if (albumOnChair) { box(f, ax + 1, ay - 2, 15, 11, { 150, 58, 70 }); f.rect(ax + 2, ay - 1, 3, 9, C(118, 40, 54)); f.rect(ax + 8, ay + 1, 5, 4, C(222, 184, 92)); f.set(ax + 10, ay + 2, C(240, 160, 176)); }
}
