#include "ui/SessionPlayState.h"
#include "ui/KitchenRound.h"
#include "ui/People.h"
#include "ui/PixelFont.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
using People::drawKid; using People::drawMom;
namespace {
const RGB OUT{ 42, 26, 28 };
const RGB WALL{ 150, 98, 60 }, WALL2{ 140, 90, 56 }, WALL_L{ 176, 120, 76 }, SEAM{ 104, 66, 44 }, BEAM{ 96, 60, 42 };
const RGB FLOOR{ 164, 110, 68 }, FLOOR2{ 154, 102, 62 }, FLOOR_L{ 184, 130, 84 }, FLOOR_S{ 112, 72, 46 };
const RGB SKY[5] = { {116,100,170}, {160,112,168}, {222,128,140}, {248,162,120}, {254,204,132} };
const RGB PINE{ 70, 52, 96 }, PINE_D{ 54, 40, 78 };
const RGB SKIN{ 246, 204, 164 }, SKIN_D{ 222, 168, 132 }, HAIR{ 110, 70, 44 }, HAIR_D{ 84, 52, 34 }, HAIR_L{ 140, 92, 58 };
const RGB YEL{ 250, 210, 64 }, YEL_D{ 208, 160, 44 }, SHORTS{ 72, 100, 170 }, SHOE{ 70, 44, 40 }, EYE{ 34, 26, 34 };
const RGB DRESS{ 214, 110, 124 }, DRESS_D{ 172, 78, 96 }, APRON{ 246, 240, 228 }, MOMHAIR{ 96, 58, 40 };
const RGB BEAR{ 112, 72, 46 }, BEAR_D{ 82, 50, 34 }, BEAR_L{ 146, 98, 62 }, MUZZLE{ 196, 150, 104 };
const RGB WORM{ 236, 140, 156 }, WORM_D{ 196, 96, 118 }, WORM_L{ 250, 190, 200 };
const RGB POT{ 206, 54, 58 }, POT_D{ 150, 34, 44 }, POT_L{ 236, 104, 100 };
const int FLOOR_Y = 152, CORNER = 195, VPX = 360, VPY = 100;
const int BOWL_X = 318, BOWL_Y = 216;
const int DOOR_X = 222;
int wave(int t, float period, float amp, float phase = 0) {
    return (int)std::lround(std::sin(t * 6.2832f / period + phase) * amp);
}
float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
float ease(float v) { v = clamp01(v); return v * v * (3 - 2 * v); }
float lerp(float a, float b, float u) { return a + (b - a) * clamp01(u); }
float bez(float a, float b, float c, float t) { float u = 1 - t; return u * u * a + 2 * u * t * b + t * t * c; }
int rnd(float v) { return (int)std::lround(v); }
const float PI = 3.14159265f;
const int MOM_X0 = 404, MOM_FEET = 155, DOOR_FEET = 153;
const float KID_PATH[3][2] = { {318,205}, {284,205}, {232,160} };
const float BEAR_PATH[4][2] = { {142,258}, {206,254}, {226,184}, {404,160} };
void shadow(Frame& f, int x, int y, int w) {
    int r = w / 2;
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -r; dx <= r; ++dx) {
            float e = (float)(dx * dx) / (r * r) + (float)(dy * dy);
            if (e > 1.0f) continue;
            RGB c = palette()[f.get(x + dx, y + dy)];
            f.set(x + dx, y + dy, C(mix(c, RGB{ 30, 10, 10 }, 0.31f)));
        }
}
void pieceBox(int type, std::vector<std::pair<int, int>>& cs, int& w, int& h) {
    cs = SessionPlayState::shapeOf(type);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    for (auto& c : cs) { c.first -= mx; c.second -= my; }
    w = (Mx - mx + 1) * 6; h = (My - my + 1) * 6;
}
int wallY(int x, float yc) { return (int)std::lround(VPY + (yc - VPY) * (VPX - x) / (float)(VPX - CORNER)); }
int floorLeft(int x) { return x >= CORNER ? FLOOR_Y : wallY(x, FLOOR_Y); }
void bubble(Frame& f, int cx, int y, const char* l1, const char* l2, int tx, int ty) {
    int w = std::max(Font::width(l1), Font::width(l2)) + 8, h = 20, x = cx - w / 2;
    uint8_t ol = C(OUT), paper = C(252, 250, 244);
    int bx = std::max(x + 6, std::min(x + w - 10, tx - 3));
    f.poly({ {bx, y + h - 1}, {bx + 7, y + h - 1}, {tx, ty} }, paper);
    f.line(bx, y + h, tx, ty, ol); f.line(bx + 7, y + h, tx, ty, ol);
    f.rect(x - 1, y, w + 2, h, ol); f.rect(x, y - 1, w, h + 2, ol);
    f.rect(x, y, w, h, paper);
    f.hline(bx + 1, bx + 6, y + h, paper);
    Font::center(f, l1, cx, y + 3, C(150, 58, 70));
    Font::center(f, l2, cx, y + 11, C(150, 58, 70));
}
void sparkle(Frame& f, int x, int y) {
    f.set(x, y, C(255, 255, 220));
    f.set(x - 1, y, C(250, 222, 110)); f.set(x + 1, y, C(250, 222, 110));
    f.set(x, y - 1, C(250, 222, 110)); f.set(x, y + 1, C(250, 222, 110));
}
int potPixel(int x, int y) {
    for (int s = -1; s <= 1; s += 2) {
        int hx = s * 25, hy = -20, dx = x - hx, dy = y - hy;
        if (dx * dx + dy * dy <= 16 && dx * dx + dy * dy >= 4) return 4;
    }
    if (y < -27 || y > 0 || std::abs(x) > 22) return 0;
    if (y <= -24) {
        float ex = x / 22.0f, ey = (y + 25.5f) / 3.0f;
        if (ex * ex + ey * ey <= 1.0f) {
            float ix = x / 19.0f, iy = (y + 25.5f) / 2.0f;
            return (ix * ix + iy * iy <= 1.0f) ? 5 : 2;
        }
        return 0;
    }
    int bx = 22 - (y > -4 ? (y + 4) : 0);
    if (std::abs(x) > bx) return 0;
    if (std::abs(x) == bx || y == 0) return 6;
    if ((((x + 40) / 6) + ((y + 40) / 5)) % 2 == 0 && (x + 40) % 6 >= 2 && (x + 40) % 6 <= 3 && (y + 40) % 5 >= 1 && (y + 40) % 5 <= 2)
        return 7;
    if (x < -15) return 3;
    if (x > 14 || y > -4) return 1;
    return 8;
}
uint8_t potColor(int code) {
    switch (code) {
    case 1: return C(POT_D); case 2: return C(POT_L); case 3: return C(POT_L); case 4: return C(OUT);
    case 5: return C(70, 22, 30); case 6: return C(OUT); case 7: return C(252, 240, 236); default: return C(POT);
    }
}
void drawPot(Frame& f, int bx, int by, float ang) {
    float ca = std::cos(ang), sa = std::sin(ang);
    for (int y = -40; y <= 12; ++y) for (int x = -40; x <= 40; ++x) {
        float lx = x * ca + y * sa, ly = -x * sa + y * ca;
        int code = potPixel((int)std::lround(lx), (int)std::lround(ly));
        if (code) f.set(bx + x, by + y, potColor(code));
    }
}
}
void KitchenRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    drawRoom(f);
    drawLeftWindow(f);
    drawFrontWindow(f);
    drawDoor(f);
    drawKitchen(f, t);
    drawCommonUI(f);
    drawTankLife(f, t);
}
void KitchenRound::drawRoom(Frame& f) {
    uint8_t wall = C(WALL), wall2 = C(WALL2), wallL = C(WALL_L), seam = C(SEAM);
    for (int y = 0; y < FLOOR_Y; ++y) {
        int k = y % 7, band = y / 7;
        for (int x = CORNER; x < 480; ++x) {
            uint8_t c = (band & 1) ? wall2 : wall;
            if (k == 0) c = seam; else if (k == 1) c = wallL;
            f.set(x, y, c);
        }
    }
    for (int x = 0; x < CORNER; ++x) {
        int yb = floorLeft(x);
        for (int y = 0; y < yb; ++y) {
            float yc = VPY + (y - VPY) * (float)(VPX - CORNER) / (VPX - x);
            int iy = (int)std::floor(yc), k = ((iy % 7) + 7) % 7, band = (int)std::floor(yc / 7.0f);
            uint8_t c = (band & 1) ? wall2 : wall;
            if (k == 0) c = seam; else if (k == 1) c = wallL;
            f.set(x, y, c);
        }
    }
    for (int k = 0; k < 70; ++k) {
        int x = CORNER + 6 + (k * 67) % 280, y = 3 + ((k * 29) % 21) * 7;
        if (y < FLOOR_Y - 2) f.hline(x, x + 3 + k % 4, y + 3, seam);
    }
    for (int k = 0; k < 9; ++k) {
        int x = 210 + (k * 97) % 260, y = 10 + (k * 41) % 130;
        f.ellipse(x, y, 2, 1, seam); f.set(x, y, C(BEAM));
    }
    uint8_t beam = C(BEAM), beamL = C(128, 84, 56);
    f.rect(0, 0, 480, 3, beam);
    f.rect(CORNER, 0, 5, FLOOR_Y, beam); f.vline(CORNER + 1, 0, FLOOR_Y - 1, beamL);
    f.rect(340, 0, 5, FLOOR_Y, beam); f.vline(341, 0, FLOOR_Y - 1, beamL);
    uint8_t fl = C(FLOOR), fl2 = C(FLOOR2), flL = C(FLOOR_L), fs = C(FLOOR_S);
    int seams[16], n = 0;
    for (float y = FLOOR_Y, step = 5; y < 270 && n < 16; y += step, step *= 1.18f) seams[n++] = (int)y;
    for (int x = 0; x < 480; ++x) {
        int top = floorLeft(x);
        for (int y = top; y < 270; ++y) {
            int row = 0;
            while (row + 1 < n && seams[row + 1] <= y) ++row;
            uint8_t c = (row & 1) ? fl2 : fl;
            if (y == seams[row]) c = fs; else if (y == seams[row] + 1) c = flL;
            f.set(x, y, c);
        }
        f.set(x, top, fs);
    }
    for (int r = 1; r < n; ++r) {
        int y0 = seams[r - 1] + 1, y1 = (r < n ? seams[r] : 270) - 1;
        if (r == n - 1) y1 = std::min(269, seams[r - 1] + 30);
        for (int j = 0; j < 5; ++j) {
            int x = (r * 71 + j * 113) % 480;
            if (y0 >= floorLeft(x)) f.vline(x, y0, y1, fs);
        }
    }
    f.ditherEllipse(292, 70, 60, 44, C(176, 122, 80));
    f.ditherEllipse(300, 200, 120, 40, C(178, 124, 78));
    f.ellipse(322, 242, 92, 30, C(120, 36, 44));
    f.ellipse(322, 241, 90, 28, C(168, 56, 62));
    f.ellipse(322, 241, 74, 21, C(226, 168, 120)); f.ellipse(322, 241, 72, 19, C(168, 56, 62));
    for (int k = 0; k < 12; ++k) {
        float a = k * 0.5236f;
        f.set(322 + (int)(std::cos(a) * 88), 241 + (int)(std::sin(a) * 27), C(240, 224, 200));
    }
    for (int y = 0; y < FLOOR_Y; y += 2) f.set(CORNER - 1, y, C(SEAM));
}
void KitchenRound::drawLeftWindow(Frame& f) {
    const float Y0 = 64, Y1 = 118, MID = 90;
    const int X0 = 16, X1 = 77, XM = 47;
    uint8_t ol = C(OUT), frame = C(196, 146, 94), frameD = C(120, 80, 52);
    for (int x = X0 - 3; x <= X1 + 3; ++x) {
        int ya = wallY(x, Y0) - 3, yb = wallY(x, Y1) + 3;
        f.vline(x, ya, yb, (x < X0 - 1 || x > X1 + 1) ? ol : frame);
        f.set(x, ya, ol); f.set(x, yb, ol);
    }
    for (int x = X0; x <= X1; ++x) {
        int ya = wallY(x, Y0), yb = wallY(x, Y1);
        for (int y = ya; y <= yb; ++y) {
            float u = (float)(y - ya) / std::max(1, yb - ya);
            int k = std::min(4, (int)(u * 5));
            if (u * 5 - k > 0.85f && k < 4 && ((x + y) & 1)) ++k;
            f.set(x, y, C(SKY[k]));
        }
    }
    f.disc(64, wallY(64, 108), 5, C(255, 236, 170));
    const int PX[6] = { 20, 31, 44, 55, 66, 75 }, PH[6] = { 22, 34, 26, 38, 24, 30 };
    for (int x = X0; x <= X1; ++x) {
        int ya = wallY(x, Y0), yb = wallY(x, Y1);
        for (int y = ya; y <= yb; ++y)
            for (int k = 0; k < 6; ++k) {
                int ta = wallY(PX[k], Y0), tb = wallY(PX[k], Y1);
                int tip = tb - PH[k] * (tb - ta) / 54, d = y - tip;
                if (d < 0) continue;
                int hw = d * 3 / 10 + ((d % 6) < 3 ? 0 : -1) + 1;
                if (std::abs(x - PX[k]) <= hw) { f.set(x, y, (k % 2) ? C(PINE_D) : C(PINE)); break; }
            }
    }
    for (int x = X0; x <= X1; ++x) f.set(x, wallY(x, MID), frameD), f.set(x, wallY(x, MID) + 1, frame);
    for (int y = wallY(XM, Y0); y <= wallY(XM, Y1); ++y) { f.set(XM, y, frame); f.set(XM + 1, y, frameD); }
    for (int x = X0 - 6; x <= X1 + 6; ++x) {
        int y = wallY(x, Y1) + 3;
        f.vline(x, y, y + 3, frame); f.set(x, y + 4, ol); f.set(x, y, C(214, 170, 112));
    }
    f.line(22, wallY(22, Y0) + 5, 27, wallY(27, Y0) + 1, C(255, 240, 220));
}
void KitchenRound::drawFrontWindow(Frame& f) {
    const int x0 = 256, y0 = 46, x1 = 338, y1 = 106;
    uint8_t ol = C(OUT), frame = C(196, 146, 94), frameD = C(120, 80, 52);
    f.rect(x0 - 3, y0 - 3, x1 - x0 + 7, y1 - y0 + 7, ol);
    f.rect(x0 - 2, y0 - 2, x1 - x0 + 5, y1 - y0 + 5, frame);
    for (int y = y0; y < 74; ++y) {
        int k = (y - y0) * 5 / 28;
        for (int x = x0; x <= x1; ++x) {
            int kk = k;
            if ((y - y0) % 6 == 5 && k < 4 && ((x + y) & 1)) ++kk;
            f.set(x, y, C(SKY[std::min(4, kk)]));
        }
    }
    f.disc(318, 70, 6, C(255, 236, 170)); f.disc(318, 70, 4, C(255, 250, 220));
    for (int x = x0; x <= x1; ++x) {
        int h = 3 + (x * 7) % 5 + ((x % 11 == 0) ? 3 : 0);
        if (x < 300) f.vline(x, 72 - h, 73, C(PINE));
    }
    f.rect(x0, 74, x1 - x0 + 1, 12, C(124, 112, 170));
    for (int y = 76; y < 86; y += 2) f.hline(310 - (y - 74) / 2, 326 + (y - 74) / 2, y, C(250, 196, 140));
    f.hline(x0, x1, 74, C(200, 140, 150));
    f.rect(x0, 86, x1 - x0 + 1, 10, C(222, 196, 150));
    f.rect(x0, 96, x1 - x0 + 1, 11, C(108, 140, 90));
    f.poly({ {286,86}, {294,86}, {300,106}, {280,106} }, C(196, 160, 112));
    f.poly({ {306,89}, {326,89}, {323,92}, {309,92} }, C(140, 70, 60));
    f.vline(297, y0, y1, frame); f.vline(298, y0, y1, frameD);
    f.hline(x0, x1, 76, frame); f.hline(x0, x1, 77, frameD);
    f.rect(x0 - 6, y1 + 3, x1 - x0 + 13, 3, frame); f.hline(x0 - 6, x1 + 6, y1 + 6, ol);
    f.line(262, 50, 266, 47, C(255, 240, 220));
    f.disc(311, 26, 11, ol); f.disc(311, 26, 10, C(150, 96, 60)); f.disc(311, 26, 8, C(248, 240, 222));
    for (int k = 0; k < 12; ++k) f.set(311 + (int)(std::cos(k * 0.5236f) * 7), 26 + (int)(std::sin(k * 0.5236f) * 7), C(150, 120, 100));
    f.line(311, 26, 311, 21, C(OUT)); f.line(311, 26, 315, 28, C(OUT));
    f.vline(290, 0, 30, C(OUT));
    f.ditherEllipse(290, 42, 26, 18, C(196, 136, 88));
    f.poly({ {284,30}, {296,30}, {302,40}, {278,40} }, C(OUT));
    f.poly({ {285,31}, {295,31}, {300,39}, {280,39} }, C(232, 138, 96));
    f.hline(281, 299, 38, C(204, 110, 80));
    f.disc(290, 42, 3, C(255, 236, 170)); f.set(289, 41, C(255, 252, 230));
    f.rect(242, 98, 3, 2, C(OUT));
    f.poly({ {241,100}, {247,100}, {249,123}, {239,123} }, C(OUT));
    f.poly({ {242,101}, {246,101}, {248,122}, {240,122} }, C(80, 110, 170));
    f.vline(242, 103, 121, C(110, 140, 200));
}
void KitchenRound::drawDoor(Frame& f) {
    const int x0 = 205, y0 = 92, x1 = 238;
    uint8_t ol = C(OUT), wood = C(132, 84, 56), woodD = C(104, 64, 44), woodL = C(160, 108, 70);
    f.rect(x0 - 2, y0 - 2, x1 - x0 + 5, FLOOR_Y - y0 + 2, ol);
    f.rect(x0 - 1, y0 - 1, x1 - x0 + 3, FLOOR_Y - y0 + 1, C(96, 60, 42));
    int cc = c();
    int open = 0;
    if (cc >= DOOR_OPEN && cc < DOOR_CLOSE) open = std::min(3, (cc - DOOR_OPEN) / 2 + 1);
    else if (cc >= DOOR_CLOSE && cc < DOOR_CLOSE + 3) open = 3 - (cc - DOOR_CLOSE + 1);
    if (open > 0) {
        for (int y = y0; y < FLOOR_Y; ++y) {
            int k = std::min(4, (y - y0) * 5 / 40);
            f.hline(x0, x1, y, C(SKY[k]));
        }
        f.rect(x0, 134, x1 - x0 + 1, FLOOR_Y - 134, C(108, 140, 90));
        for (int x = x0; x <= x1; x += 3) f.vline(x, 128 - (x % 7), 134, C(PINE));
        int w = (x1 - x0) * (3 - open) / 3;
        f.rect(x0, y0, std::max(3, w), FLOOR_Y - y0, wood);
        f.vline(x0 + std::max(3, w), y0, FLOOR_Y - 1, ol);
    } else {
        f.rect(x0, y0, x1 - x0 + 1, FLOOR_Y - y0, wood);
        f.rect(x0 + 4, y0 + 5, x1 - x0 - 7, 22, woodD); f.rect(x0 + 5, y0 + 6, x1 - x0 - 9, 20, woodL);
        f.rect(x0 + 4, y0 + 32, x1 - x0 - 7, 24, woodD); f.rect(x0 + 5, y0 + 33, x1 - x0 - 9, 22, woodL);
        f.disc(x1 - 4, y0 + 30, 1, C(236, 200, 110));
    }
    f.rect(x0, FLOOR_Y - 1, x1 - x0 + 1, 2, C(150, 50, 56));
}
void KitchenRound::drawKitchen(Frame& f, int t) {
    uint8_t ol = C(OUT), wood = C(156, 104, 62), woodD = C(114, 72, 50), woodL = C(196, 146, 94);
    drawTank(f, t);
    f.rect(352, 72, 128, 4, ol); f.rect(353, 72, 127, 3, wood); f.hline(353, 479, 72, woodL);
    for (int bx : { 362, 470 }) { f.line(bx, 76, bx + 6, 76, ol); f.line(bx, 76, bx, 82, ol); f.line(bx, 82, bx + 6, 76, ol); }
    for (int k = 0; k < 3; ++k) { f.vline(368 + k * 5, 84, 100, C(170, 180, 196)); f.set(368 + k * 5, 101, C(120, 130, 146)); }
    f.vline(458, 83, 88, ol); f.disc(458, 94, 7, ol); f.disc(458, 94, 6, C(60, 60, 70)); f.set(456, 92, C(110, 110, 124));
    f.vline(473, 83, 86, ol); f.disc(473, 90, 5, ol); f.disc(473, 90, 4, C(60, 60, 70));
    f.rect(350, 133, 130, 5, ol); f.rect(351, 134, 129, 3, C(236, 226, 206)); f.hline(351, 479, 134, C(250, 246, 236));
    f.rect(352, 138, 128, FLOOR_Y - 138, ol);
    f.rect(353, 139, 127, FLOOR_Y - 139, wood);
    for (int dx : { 356, 384, 448 }) for (int dy : { 142 }) {
        f.rect(dx, dy, 24, 7, woodD); f.rect(dx + 1, dy + 1, 22, 5, woodL); f.hline(dx + 9, dx + 14, dy + 3, C(236, 200, 110));
    }
    f.rect(413, 139, 33, 13, C(70, 66, 76));
    f.rect(415, 141, 29, 9, C(30, 28, 36));
    f.rect(413, 132, 33, 2, C(80, 80, 92));
    f.rect(360, 129, 26, 4, ol); f.rect(361, 130, 24, 2, C(206, 160, 108));
    f.ellipse(368, 127, 5, 3, ol); f.ellipse(368, 127, 4, 2, C(214, 152, 84)); f.hline(366, 370, 126, C(236, 188, 120));
    f.disc(380, 128, 2, C(250, 222, 80));
}
void KitchenRound::drawTank(Frame& f, int t) {
    const int x0 = 370, y0 = 34, x1 = 458, y1 = 71;
    uint8_t ol = C(OUT);
    f.rect(x0, y0, x1 - x0 + 1, y1 - y0 + 1, ol);
    f.rect(x0 + 1, y0 + 1, x1 - x0 - 1, 3, C(60, 70, 90));
    for (int y = y0 + 4; y < y1 - 4; ++y)
        for (int x = x0 + 2; x < x1 - 1; ++x)
            f.set(x, y, (y < y0 + 9 && ((x + y) & 1)) ? C(150, 210, 220) : C(102, 170, 190));
    f.rect(x0 + 2, y1 - 4, x1 - x0 - 3, 3, C(200, 180, 140));
    for (int x = x0 + 3; x < x1 - 2; x += 3) f.set(x, y1 - 4, C(160, 140, 110));
    f.ellipse(414, y1 - 5, 5, 3, C(140, 140, 150)); f.set(412, y1 - 7, C(190, 190, 200));
    for (int k = 0; k < 3; ++k) {
        int px = 378 + k * 34 + (k == 2 ? 12 : 0);
        for (int y = y1 - 5; y > y0 + 14 + k * 4; --y) f.set(px + (int)(std::sin(y * 0.5f + t * 0.2f + k) * 1.5f), y, C(80, 150, 90));
    }
    f.line(x0 + 4, y0 + 8, x0 + 8, y0 + 5, C(230, 250, 255));
    for (int k = 0; k < 4; ++k) {
        int age = (t + k * 9) % 30, bx = 400 + k * 12 + (age / 6) % 2, by = y1 - 6 - age;
        if (by > y0 + 5) f.set(bx, by, C(220, 245, 250));
    }
}
void KitchenRound::drawTable(Frame& f) {
    uint8_t ol = C(OUT), red = C(200, 56, 60), pink = C(232, 150, 150), white = C(250, 242, 234);
    auto ging = [&](int x, int y, int cw, int ch) -> uint8_t {
        bool a = ((x / cw) & 1), b = ((y / ch) & 1);
        return (a && b) ? red : (a || b) ? pink : white;
    };
    f.rect(234, 246, 4, 16, C(96, 60, 42)); f.rect(398, 246, 4, 16, C(96, 60, 42));
    f.ellipse(318, 262, 90, 4, C(120, 36, 44));
    f.poly({ {238,203}, {398,203}, {411,227}, {225,227} }, ol);
    for (int y = 204; y < 227; ++y) {
        float u = (y - 204) / 23.0f;
        int xl = 239 - (int)(u * 13), xr = 397 + (int)(u * 13);
        for (int x = xl; x <= xr; ++x) {
            int gx = (int)((x - 318) / (0.8f + 0.4f * u) + 400);
            f.set(x, y, ging(gx, (int)(u * 8 * 3), 8, 3));
        }
    }
    for (int y = 227; y < 247; ++y)
        for (int x = 225; x <= 411; ++x) {
            int yy = y + ((x / 6) % 2) * (y > 244 ? 1 : 0);
            f.set(x, y, ging(x - 225 + 400, yy - 227 + 30, 8, 5));
        }
    for (int x = 225; x <= 411; ++x) {
        int d = (x / 6) % 2;
        f.set(x, 247 + d, ol); f.set(x, 246 + d, d ? pink : white);
    }
    f.vline(224, 227, 247, ol); f.vline(412, 227, 247, ol);
    f.hline(226, 410, 227, C(180, 120, 120));
}
void KitchenRound::drawChair(Frame& f, int slide) {
    int y = 164 - slide * 3;
    uint8_t ol = C(OUT), wood = C(156, 104, 62), woodL = C(196, 146, 94), woodD = C(114, 72, 50);
    for (int px : { 302, 331 }) { f.rect(px - 1, y - 1, 7, 46, ol); f.rect(px, y, 5, 44, wood); f.vline(px, y, y + 43, woodL); }
    f.rect(301, y - 1, 36, 9, ol); f.rect(302, y, 34, 7, wood); f.hline(302, 335, y, woodL); f.hline(302, 335, y + 6, woodD);
    for (int k = 0; k < 4; ++k) { int sx = 309 + k * 6; f.rect(sx - 1, y + 7, 5, 22, ol); f.rect(sx, y + 7, 3, 22, wood); f.vline(sx, y + 7, y + 28, woodL); }
    f.rect(301, y + 28, 36, 4, ol); f.rect(302, y + 29, 34, 2, woodD);
}
void KitchenRound::drawBowl(Frame& f, int x, int y, int eaten) {
    uint8_t ol = C(OUT), glaze = C(178, 46, 52), glazeD = C(130, 30, 40), glazeL = C(222, 96, 96);
    f.ellipse(x, y + 4, 21, 8, ol); f.ellipse(x, y + 4, 20, 7, glaze);
    f.ellipse(x + 4, y + 7, 14, 3, glazeD);
    f.ellipse(x, y - 1, 23, 6, ol); f.ellipse(x, y - 1, 22, 5, glazeL);
    f.ellipse(x, y - 1, 19, 3, C(236, 226, 206));
    f.set(x - 12, y - 3, C(252, 248, 236)); f.set(x + 9, y, C(220, 206, 180));
    int e = std::min(eaten, 10);
    int cut = x - 12 + e * 20 / 10;
    uint8_t flesh = C(214, 208, 200), fleshD = C(176, 168, 160), bone = C(250, 248, 240), skin = C(150, 156, 170);
    f.poly({ {x - 16, y - 5}, {x - 12, y - 2}, {x - 16, y + 1} }, e >= 1 ? bone : skin);
    for (int i = x - 12; i <= x + 6; ++i) {
        int hh = (int)(3 * std::sin((i - (x - 12)) * 3.14159f / 20) + 1);
        if (i < cut) { f.set(i, y - 2, bone); if ((i % 3) == 0) { f.set(i, y - 3, bone); f.set(i, y - 1, bone); } }
        else { f.vline(i, y - 2 - hh, y - 2 + hh, flesh); f.set(i, y - 2 + hh, fleshD); f.set(i, y - 2 - hh, skin); }
    }
    f.ellipse(x + 9, y - 2, 4, 3, skin); f.ellipse(x + 9, y - 2, 3, 2, C(196, 200, 212));
    f.set(x + 10, y - 3, C(EYE)); f.set(x + 13, y - 1, fleshD);
    if (e >= 10) {
        f.line(x + 6, y - 4, x + 20, y - 12, C(196, 200, 212)); f.ellipse(x + 5, y - 3, 3, 2, C(196, 200, 212));
    }
}
#define RUG_COLORS { {'O', {42, 26, 28}}, {'F', {112, 72, 46}}, {'f', {82, 50, 34}}, {'G', {146, 98, 62}}, \
                     {'T', {236, 226, 206}}, {'M', {196, 150, 104}}, {'N', {50, 30, 30}}, {'E', {42, 26, 28}} }
static const Sprite RUG_PELT("rug pelt", {
    "......OOOOOOOO......................................................OOOOOOOO........",
    ".....OFTFTFTFFO....................................................OFFTFTFTFO.......",
    ".....OFFFFFFFFFO..................................................OFFFFFFFFFO.......",
    "......OFFFFFFFFFO................................................OFFFFFFFFFO........",
    "........OFFFFFFFFO..............................................OFFFFFFFFO..........",
    ".........OFFFFFFFFOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOFFFFFFFFFO...........",
    "..........OFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO...........",
    ".........OFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO.........",
    "........OFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO........",
    ".......OFFFFFFFFFFFFFFFFFFFFFfFFFFFFFFFFFfFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO......",
    ".......OFFFfFFFFFFFfFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFfFFFFFFFFfFFFFFO......",
    ".......OFFFFFFFFFFFFFFFFFFFGGGGGGGGGGGGGFFFFGGGGGGGGGGGGGFFFFFFFFFFFFFFFFFFFFO......",
    ".......OFFFFFFFFfFFFFFFFFGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGFFFFFFFFfFFFFFFFFO......",
    "...OOOOOFFFFFFFFFFFFFFFFFGGGfGGGGGGGGfGGGGGGGGGGGGGGGGGGGGGGFFFFFFFFFFFFFFFFFO......",
    "..OfFFFFFFFFFFFFFFFFFfFFFGGGGGGGGGGGGGGGGGGGGGGGfGGGGGGGGfGGGGFFFFFfFFFFFFFFFO......",
    "...OOOOOFFFFFFFFFFFFFFFFFGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGFFFFFFFFFFFFFFFFFO......",
    ".......OFFFfFFFFFFFfFFFFFGGGfGGGGGGGGfGGGGGGGGGGGGGGGGGGGGGGFFFFFFFFfFFFFFFFFO......",
    ".......OFFFFFFFFFFFFFFFFFFFGGGGGGGGGGGGGFFFFGGGGGGGGGGGGGFFFFFFFFFFFFFFFFFFFFO......",
    ".......OFFFFFFFFfFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO.......",
    ".......OFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFfFFFFFFFFFFFFFFFFFFFFFFFO.......",
    ".......OFFFfFFFFFFFfFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO.......",
    "........OFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFO........",
    ".........OfFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFfO.........",
    "..........OffFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFffO..........",
    ".........OFFFFFFFFOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOFFFFFFFFFO...........",
    "........OFFFFFFFFO..............................................OFFFFFFFFO..........",
    "......OFFFFFFFFFO................................................OFFFFFFFFFO........",
    ".....OFFFFFFFFFO..................................................OFFFFFFFFFO.......",
    ".....OFTFTFTFFO....................................................OFFTFTFTFO.......",
    "......OOOOOOOO......................................................OOOOOOOO........",
}, RUG_COLORS);
static const Sprite RUG_HEAD("rug head", {
    "...OOOO..........OOOO...",
    "..OFffFO........OFffFO..",
    "..OFfFFOOOOOOOOOOFFfFO..",
    "...OFFFFFFFFFFFFFFFFO...",
    "..OFFGGFFFFFFFFFFFFFFO..",
    ".OFFGGGFFFFFFFFFFFFFFFO.",
    ".OFFFFFFFFFFFFFFFFFFFFO.",
    ".OFFFEEEFFFFFFFFEEEFFFO.",
    ".OFFFFFFMMMMMMMMFFFFFFO.",
    ".OFFFFMMMMMMMMMMMMFFFFO.",
    ".OFFFFMMMMNNNNMMMMFFFFO.",
    ".OFFFFMMMMMNNMMMMMFFFFO.",
    "..OFFFMMMMMNNMMMMMFFFO..",
    "..OFFFMMMNNMMNNMMMFFFO..",
    "...OFFFMMMMMMMMMMFFFO...",
    "....OFFFFMMMMMMFFFFO....",
    ".....OOFFFFFFFFFFOO.....",
    ".......OOOOOOOOOO.......",
}, RUG_COLORS);
void KitchenRound::drawRug(Frame& f, int c, int t) {
    const int PX = 92, PY = 230;
    bool awake = c >= 4, sitting = c >= 8;
    int up = sitting ? std::min(10, (c - 8) * 2) : 0;
    blit(f, RUG_PELT, PX, PY);
    int hx = PX + 70, hy = PY + 6 - up;
    if (up > 0) {
        uint8_t ol = C(OUT), fur = C(112, 72, 46);
        f.taper(PX + 72, PY + 16, hx + 12, hy + 14, 15, 13, ol); f.taper(PX + 72, PY + 16, hx + 12, hy + 14, 13, 11, fur);
    }
    blit(f, RUG_HEAD, hx, hy);
    if (awake && t % 40 >= 2) {
        int look = c >= 29 ? 1 : 0;
        uint8_t fur = C(112, 72, 46), ink = C(OUT), white = C(250, 250, 250);
        for (int ex : { 5, 16 }) {
            f.hline(hx + ex, hx + ex + 2, hy + 7, fur);
            f.rect(hx + ex + look, hy + 6, 2, 2, ink); f.set(hx + ex + look, hy + 6, white);
        }
    }
    if (c >= 29 && c < 39) {
        int pop = c < 31 ? 2 : 0, x = hx + 12, y = hy - 4;
        f.rect(x - 2 - pop / 2, y - 16 - pop, 5 + pop, 10 + pop, C(OUT)); f.rect(x - 1, y - 15 - pop, 3, 8 + pop, C(252, 250, 244));
        f.rect(x - 2, y - 4, 5, 4, C(OUT)); f.rect(x - 1, y - 3, 3, 2, C(252, 250, 244));
        f.vline(x, y - 14 - pop, y - 9, C(200, 56, 60)); f.set(x, y - 2, C(200, 56, 60));
    }
}
void KitchenRound::drawBear(Frame& f, int x, int y, int facing, int step, int reach, bool blink) {
    uint8_t ol = C(OUT), fur = C(BEAR), furD = C(BEAR_D), furL = C(BEAR_L), muz = C(MUZZLE);
    int h = 30 + (y - 160) * 10 / 100;
    h = std::max(30, std::min(40, h));
    int s = h * 10 / 36;
    int sw = (step >= 0) ? ((step % 2) ? 1 : -1) * s / 4 : 0;
    for (int k = -1; k <= 1; k += 2) {
        int lx = x + k * s / 2 + (k < 0 ? sw : -sw);
        f.rect(lx - s / 3 - 1, y - s - 1, 2 * s / 3 + 2, s + 1, ol); f.rect(lx - s / 3, y - s, 2 * s / 3, s, fur);
        f.hline(lx - s / 3, lx + s / 3 - 1, y - 1, furD);
    }
    int by = y - s - s * 6 / 5;
    f.ellipse(x, by, s + s / 4 + 1, s * 7 / 5 + 1, ol); f.ellipse(x, by, s + s / 4, s * 7 / 5, fur);
    if (facing == 3 || facing == 1 || facing == 0) f.ellipse(x + (facing == 1 ? 2 : facing == 0 ? -2 : 0), by + 2, s * 2 / 3, s * 4 / 5, furL);
    int ay = by - s / 2;
    for (int k = -1; k <= 1; k += 2) {
        int ex = x + k * (s + s / 3), ey = ay + s - (k > 0 ? 0 : 0) + (k < 0 ? -sw : sw);
        if (k > 0 && reach > 0) { ex = x + s + reach * s / 2; ey = ay - reach * s / 4; }
        if (facing == 0 && k > 0 && reach > 0) ex = x - s - reach * s / 2;
        f.taper(x + k * s, ay, ex, ey, s * 2 / 3 + 2, s / 2 + 2, ol); f.taper(x + k * s, ay, ex, ey, s * 2 / 3, s / 2, fur);
    }
    int hy = by - s * 2, hx = x + (facing == 1 ? s / 4 : facing == 0 ? -s / 4 : 0);
    f.disc(hx - s * 2 / 3, hy - s * 2 / 3, s / 3 + 1, ol); f.disc(hx + s * 2 / 3, hy - s * 2 / 3, s / 3 + 1, ol);
    f.disc(hx - s * 2 / 3, hy - s * 2 / 3, s / 3, fur); f.disc(hx + s * 2 / 3, hy - s * 2 / 3, s / 3, fur);
    f.disc(hx, hy, s + 1, ol); f.disc(hx, hy, s, fur); f.ellipse(hx - s / 3, hy - s / 3, s / 2, s / 3, furL);
    if (facing == 2) { f.vline(hx, hy - s / 2, hy + s / 2, furD); return; }
    int mx = hx + (facing == 1 ? s / 2 : facing == 0 ? -s / 2 : 0);
    f.ellipse(mx, hy + s / 3, s / 2, s / 3, muz); f.rect(mx - 1, hy + s / 4 - 1, 3, 2, ol);
    int ex = facing == 1 ? s / 4 : facing == 0 ? -s / 4 : 0;
    if (blink) { f.hline(hx - s / 2 + ex - 1, hx - s / 2 + ex + 1, hy - 1, ol); f.hline(hx + s / 2 + ex - 1, hx + s / 2 + ex + 1, hy - 1, ol); }
    else { f.rect(hx - s / 2 + ex, hy - 2, 2, 2, ol); f.rect(hx + s / 2 + ex - 1, hy - 2, 2, 2, ol); }
}
void KitchenRound::drawWorm(Frame& f, int x, int y, int t, bool facingLeft, int segCount) {
    uint8_t ol = C(OUT), pink = C(WORM), pinkD = C(WORM_D), pinkL = C(WORM_L);
    int sway = wave(t, 10, 1.0f), d = facingLeft ? -1 : 1;
    for (int k = std::min(7, segCount); k >= 1; --k) {
        int bx = x - d * k * 3 + (k % 2) * sway, by = y + k * 4;
        int r = std::max(2, 4 - k / 3);
        f.disc(bx, by, r + 1, ol); f.disc(bx, by, r, pink); f.set(bx - 1, by - 1, pinkL); f.hline(bx - r + 1, bx + r - 1, by + 2, pinkD);
    }
    f.poly({ {x - d * 3, y + 2}, {x - d * 12, y - 5}, {x - d * 10, y + 1}, {x - d * 14, y + 4}, {x - d * 8, y + 5} }, C(110, 70, 140));
    f.disc(x, y, 5, ol); f.disc(x, y, 4, pink); f.set(x - 1, y - 2, pinkL);
    f.disc(x + d * 2, y - 1, 2, ol); f.set(x + d * 2, y - 1, C(250, 250, 250));
    if (t % 40 < 2) f.hline(x + d * 2 - 1, x + d * 2 + 1, y - 1, ol);
    f.disc(x + d, y - 6, 2, C(214, 40, 56)); f.set(x + d, y - 7, C(240, 110, 120)); f.set(x + d * 3, y - 5, C(70, 130, 70));
}
void KitchenRound::drawFish(Frame& f, float fx, float fy, int col, int s, int t) {
    const RGB* r = ToyBlocks::ramp(col);
    uint8_t a = C(r[2]), b = C(r[1]);
    int x = rnd(fx), y = rnd(fy);
    static const int W0[5] = { 1, 0, 0, 0, 1 }, W1[5] = { 3, 4, 5, 4, 3 };
    for (int yy = -2; yy <= 2; ++yy)
        for (int xx = W0[yy + 2]; xx <= W1[yy + 2]; ++xx) f.set(x + xx * s, y + yy, yy < 1 ? a : b);
    static const int BODY[11][2] = { {0,-1},{1,-2},{2,-2},{3,-2},{4,-1},{5,0},{4,1},{3,2},{2,2},{1,2},{0,1} };
    for (auto& p : BODY) f.set(x + p[0] * s, y + p[1], b);
    int flap = (t / 2) % 2;
    for (int yy : { -2 + flap, -1, 0, 1, 2 - flap }) f.set(x - s, y + yy, b);
    f.set(x - 2 * s, y - 2 + flap, b); f.set(x - 2 * s, y + 2 - flap, b);
    f.set(x + 3 * s, y - 1, C(255, 255, 255)); f.set(x + 4 * s, y - 1, C(20, 20, 30));
}
void KitchenRound::drawTankLife(Frame& f, int t) {
    int cc = c();
    for (size_t i = 0; i < fish_.size(); ++i) {
        const Fish& fi = fish_[i];
        float span = 74, speed = 0.8f + (i * 37 % 10) * 0.08f, phase = (i * 53 % 20) / 10.0f;
        float tt = std::fmod((t - fi.born) * speed / span + phase, 2.0f), x; int facing;
        if (tt < 1) { x = 376 + span * (0.5f - 0.5f * std::cos(PI * tt)); facing = 1; }
        else { x = 450 - span * (0.5f - 0.5f * std::cos(PI * (tt - 1))); facing = -1; }
        float y = 44 + (i * 7) % 15 + std::sin((t - fi.born) * 0.21f + i) * 1.5f;
        float k = (t - fi.born) / 8.0f;
        if (k < 1) { x = fi.sx + (x - fi.sx) * ease(k); y = 42 + (y - 42) * ease(k); }
        drawFish(f, x, y, fi.col, facing, t + (int)i);
    }
    for (size_t s = 0; s < splashes_.size(); ++s) {
        int k = t - splashes_[s], sx = rnd(splashX_[s]);
        if (k < 0 || k >= 5) continue;
        for (int a = -1; a <= 1; a += 2) for (int j = 0; j < 3; ++j) f.set(sx + a * (2 + k * 2 + j), 40 - (3 - std::abs(k - 2)) + j, C(230, 250, 255));
        f.hline(sx - 3 - k, sx + 3 + k, 40, C(220, 246, 252));
    }
    bool on = cc < KNOB;
    if (on) {
        for (int x = 422; x <= 437; x += 3) { f.set(x, 133, C(90, 150, 250)); f.set(x + 1, 133 - (t + x) % 2, C(250, 170, 80)); }
        f.hline(419, 440, 146, C(230, 120, 60)); f.hline(419, 440, 147, C(150, 70, 40));
    }
    f.disc(449, 145, 2, C(OUT));
    if (on) f.vline(449, 143, 145, C(220, 220, 230)); else f.hline(448, 450, 145, C(220, 220, 230));
    const int x = 428, y = 57;
    uint8_t note = C(250, 226, 110), noteD = C(222, 190, 80), ink = C(70, 60, 120), red = C(200, 56, 60);
    int n = std::min(10, (int)fish_.size());
    bool pop = !fish_.empty() && t - fish_.back().born < 3 && n < 10;
    f.rect(x + 1, y + 1, 28, 24, C(110, 80, 60));
    f.rect(x, y - pop, 28, 24, note); f.hline(x, x + 27, y - pop, noteD);
    f.poly({ {x + 22, y + 23 - pop}, {x + 27, y + 18 - pop}, {x + 27, y + 23 - pop} }, noteD);
    f.set(x + 13, y + 1 - pop, red);
    text(f, "FISH", x + 2, y + 3 - pop, ink);
    text(f, std::to_string(n), x + 19, y + 3 - pop, n >= 10 ? red : ink);
    for (int i = 0; i < 10; ++i) {
        int px = x + 3 + (i % 5) * 5, py = y + 11 + (i / 5) * 6 - pop;
        if (i < n) { f.set(px, py, ink); f.set(px + 2, py, ink); f.set(px + 1, py + 1, ink); f.set(px, py + 2, ink); f.set(px + 2, py + 2, ink); }
        else f.set(px + 1, py + 1, noteD);
    }
    if (n >= 10 && !(earnedT_ >= 0 && t <= earnedT_ + 12 && t % 2))
        for (int k = 0; k < 40; ++k) { float a = k * 0.157f; f.set(x + 14 + (int)(std::cos(a) * 15), y + 12 + (int)(std::sin(a) * 11), red); }
}
void KitchenRound::bitePose(int t, float& bite, bool& chew, int& eaten) const {
    bite = 0; chew = false; eaten = 0;
    for (int b0 : bites_) {
        int k = t - b0;
        if (k >= 0 && k < 4) bite = ease(k / 3.0f);
        else if (k >= 4 && k < 6) bite = 1.0f;
        else if (k >= 6 && k < 9) bite = 1 - ease((k - 6) / 2.0f);
        else if (k >= 9 && k < 13) chew = true;
        if (k >= 4) ++eaten;
    }
}
void KitchenRound::kidPos(int cc, float& x, float& y) const {
    float d = (cc - KID_WALK0) * 1.6f;
    for (int i = 0; i < 2; ++i) {
        float dx = KID_PATH[i + 1][0] - KID_PATH[i][0], dy = KID_PATH[i + 1][1] - KID_PATH[i][1], L = std::sqrt(dx * dx + dy * dy);
        if (d <= L) { x = KID_PATH[i][0] + dx * d / L; y = KID_PATH[i][1] + dy * d / L; return; }
        d -= L;
    }
    x = KID_PATH[2][0]; y = KID_PATH[2][1];
}
void KitchenRound::bearPos(int cc, float& x, float& y, int& seg) const {
    float d = (cc - BEAR_WALK0) * 3.5f;
    for (int i = 0; i < 3; ++i) {
        float dx = BEAR_PATH[i + 1][0] - BEAR_PATH[i][0], dy = BEAR_PATH[i + 1][1] - BEAR_PATH[i][1], L = std::sqrt(dx * dx + dy * dy);
        if (d <= L) { x = BEAR_PATH[i][0] + dx * d / L; y = BEAR_PATH[i][1] + dy * d / L; seg = i; return; }
        d -= L;
    }
    x = BEAR_PATH[3][0]; y = BEAR_PATH[3][1]; seg = 2;
}
void KitchenRound::drawSoupPot(Frame& f, int t, bool stirring) {
    uint8_t ol = C(OUT), steel = C(176, 180, 196), steelL = C(220, 224, 236), steelD = C(130, 134, 150);
    f.rect(414, 121, 32, 3, ol); f.hline(415, 444, 122, C(236, 230, 214));
    if (stirring) {
        int sx = 430 + rnd(std::sin(t * 0.8f) * 5);
        f.set(sx, 122, C(214, 200, 176)); f.set(sx + 1, 122, C(196, 180, 150));
    }
    f.rect(414, 124, 32, 9, ol); f.rect(415, 124, 30, 8, steel);
    f.hline(415, 444, 124, steelL); f.hline(415, 444, 131, steelD);
    f.rect(410, 125, 4, 2, ol); f.rect(446, 125, 4, 2, ol);
    bool on = c() < KNOB;
    float steam = on ? 1.0f : std::max(0.0f, 1 - (c() - KNOB) / 8.0f);
    for (int k = 0; k < 3; ++k)
        for (int j = 0; j < 4; ++j) {
            if (j >= 4 * steam) continue;
            int y = 118 - j * 3 - (t + k * 4) % 6, x = 424 + k * 6 + rnd(std::sin((t + j * 3 + k * 2) * 0.5f));
            f.set(x, y, j < 2 ? C(255, 246, 236) : C(226, 206, 186));
        }
}
void KitchenRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    int cc = c();
    if (cc < MOM_GONE) {
        float mx = MOM_X0, my = MOM_FEET;
        if (cc < MOM_TURN) {
            drawSoupPot(f, t, false);
            drawMom(f, rnd(mx), rnd(my), People::RIGHT, -1, false, false, true);
            int shx = rnd(mx) + 1, shy = rnd(my) - 22;
            int hx = 422 + rnd(std::sin(t * 0.8f) * 3), hy = 118 + rnd(std::cos(t * 0.8f) * 1.5f);
            uint8_t skin = C(246, 204, 164), dress = C(214, 110, 124), ol = C(OUT), wood = C(170, 120, 72);
            f.taper(shx, shy, hx, hy, 4, 3, ol); f.taper(shx, shy, hx, hy, 2, 2, dress);
            f.disc(hx, hy, 2, ol); f.disc(hx, hy, 1, skin);
            int sx = 430 + rnd(std::sin(t * 0.8f) * 5);
            f.line(hx + 1, hy - 3, sx, 124, wood); f.set(hx + 1, hy - 4, ol);
            drawSoupPot(f, t, true);
        } else {
            drawSoupPot(f, t, false);
            People::Facing face = People::FRONT; int step = -1;
            bool talk = false;
            if (cc < MOM_WALK0) talk = cc >= MOM_SAY0 && cc < MOM_SAY1 && cc % 4 < 2;
            else if (cc < MOM_WALK1) { mx = std::max(240.0f, MOM_X0 - (cc - MOM_WALK0) * 3.0f); face = mx > 240 ? People::LEFT : People::BACK; step = mx > 240 ? (cc / 2) % 4 : -1; }
            else {
                float u = cc >= MOM_IN0 ? (float)(cc - MOM_IN0) / (MOM_GONE - MOM_IN0) : 0;
                mx = 240 + (DOOR_X - 240) * u; my = MOM_FEET + (DOOR_FEET - MOM_FEET) * u;
                face = People::BACK; step = cc >= MOM_IN0 ? (cc / 2) % 4 : -1;
            }
            shadow(f, rnd(mx), rnd(my) + 1, 12);
            drawMom(f, rnd(mx), rnd(my), face, step, talk, t % 37 == 0);
        }
    } else drawSoupPot(f, t, false);
    int bf = -1, bs = -1, reach = 0; float bx = 0, by = 0;
    if (cc >= BEAR_STAND) {
        bf = 3;
        if (cc < BEAR_WALK0) { bx = BEAR_PATH[0][0]; by = BEAR_PATH[0][1]; }
        else if (cc < BEAR_WALK1) { int seg; bearPos(cc, bx, by, seg); bf = seg == 1 ? 2 : 1; bs = (cc / 2) % 2; }
        else if (cc < BEAR_TURN) {
            bx = BEAR_PATH[3][0]; by = BEAR_PATH[3][1]; bf = 1;
            reach = cc < KNOB + 4 ? std::min(4, cc - BEAR_WALK1 + 1) : std::max(0, 4 - (cc - KNOB - 4));
        } else { bx = BEAR_PATH[3][0]; by = BEAR_PATH[3][1]; bf = 3; }
    }
    bool bearBlink = cc >= BEAR_TURN + 7 && cc < BEAR_TURN + 9;
    if (bf >= 0 && by <= 206) { shadow(f, rnd(bx), rnd(by) + 1, 22); drawBear(f, rnd(bx), rnd(by), bf, bs, reach, bearBlink); }
    bool standing = cc >= KID_STAND;
    drawChair(f, standing ? std::min(3, cc - KID_STAND + 1) : 0);
    if (standing && cc < KID_GONE) {
        float kx, ky; People::Facing face = People::FRONT; int step = -1;
        if (cc < KID_STAND + KID_HOP) { kx = KID_PATH[0][0]; ky = KID_PATH[0][1] + (KID_STAND + KID_HOP - cc) * 2; }
        else if (cc < KID_WALK0) { kx = KID_PATH[0][0]; ky = KID_PATH[0][1]; }
        else if (cc < KID_WALK1) { kidPos(cc, kx, ky); face = ky > 203 ? People::LEFT : People::BACK; step = (kx > KID_PATH[2][0] + 0.5f || ky > KID_PATH[2][1] + 0.5f) ? (cc / 2) % 4 : -1; }
        else if (cc < KID_IN0) { kx = KID_PATH[2][0]; ky = KID_PATH[2][1]; face = People::BACK; }
        else {
            float u = (float)(cc - KID_IN0) / (KID_GONE - KID_IN0);
            kx = KID_PATH[2][0] + (DOOR_X - KID_PATH[2][0]) * u; ky = KID_PATH[2][1] + (DOOR_FEET - KID_PATH[2][1]) * u;
            face = People::BACK; step = (cc / 2) % 4;
        }
        if (cc >= KID_STAND + KID_HOP) shadow(f, rnd(kx), rnd(ky) + 1, 12);
        drawKid(f, rnd(kx), rnd(ky), face, step);
    }
    float bite; bool chew; int eaten;
    bitePose(t, bite, chew, eaten);
    bool done = eaten >= 10 && bite == 0;
    const int KX = 310, KY = 178;
    if (!standing) {
        blit(f, People::KID_BODY_F, KX, KY + 15);
        blit(f, People::KID_HEAD_F, KX, KY);
        uint8_t eye = C(34, 26, 34), skin = C(246, 204, 164), mouth = C(176, 80, 84);
        bool talk = cc >= KID_SAY0 && cc < KID_SAY1 && cc % 4 < 2;
        if (t % 41 == 0) for (int ex : { 3, 11 }) { f.hline(KX + ex, KX + ex + 1, KY + 8, skin); f.hline(KX + ex, KX + ex + 1, KY + 9, eye); }
        if (cc >= KID_LOOK && cc < KID_STAND)
            for (int ex : { 3, 11 }) { f.hline(KX + ex, KX + ex + 1, KY + 8, skin); f.hline(KX + ex, KX + ex + 1, KY + 9, skin); f.set(KX + ex + 1, KY + 8, eye); f.set(KX + ex + 1, KY + 9, eye); }
        if (bite > 0.6f || talk) f.rect(KX + 7, KY + 11, 2, 2, mouth);
        else if (chew) f.hline(KX + 6, KX + 9, KY + 11 + (t % 2), mouth);
    }
    drawTable(f);
    drawBowl(f, BOWL_X, BOWL_Y, eaten);
    if (!standing) {
        uint8_t ol = C(OUT), shirt = C(250, 210, 64), skin = C(246, 204, 164), spoon = C(196, 200, 212);
        int shx = KX + 13, shy = KY + 18;
        int hx = done ? KX + 18 : rnd(lerp(KX + 16, KX + 11, bite)), hy = done ? KY + 34 : rnd(lerp(BOWL_Y - 4, KY + 16, bite));
        int mx2 = (shx + hx) / 2, my2 = (shy + hy) / 2;
        f.taper(shx, shy, hx, hy, 5, 4, ol); f.taper(shx, shy, mx2, my2, 3, 3, shirt); f.taper(mx2, my2, hx, hy, 3, 2, skin);
        f.disc(hx, hy, 2, ol); f.disc(hx, hy, 1, skin);
        if (!done) { int sx = hx - 4, sy = bite > 0.5f ? hy - 3 : hy + 3; f.line(hx, hy, sx, sy, spoon); f.ellipse(sx - 1, sy, 2, 1, spoon); }
        f.taper(KX + 2, shy, KX - 2, KY + 28, 5, 4, ol); f.taper(KX + 2, shy, KX, KY + 23, 3, 3, shirt); f.taper(KX, KY + 23, KX - 2, KY + 28, 3, 2, skin);
        f.disc(KX - 2, KY + 29, 2, ol); f.disc(KX - 2, KY + 29, 1, skin);
    }
    if (bf < 0) drawRug(f, cc >= BEAR0 ? cc - BEAR0 : -1, t);
    else if (by > 206) { shadow(f, rnd(bx), rnd(by) + 1, 22); drawBear(f, rnd(bx), rnd(by), bf, bs, reach, bearBlink); }
    if (cc >= THROW + THROW_LEN) {
        std::vector<std::pair<int, int>> cs; int bw, bh; pieceBox(endPiece_, cs, bw, bh);
        for (auto& q : cs) ToyBlocks::cell6(f, 210 + q.first * 6, 244 - bh + q.second * 6, endPiece_);
    }
}
void KitchenRound::drawNext(Frame& f, int piece, int t) {
    const int bx = 155, by = 52;
    int cc = c(), k = cc - WORM0;
    float ang = 0; int jx = 0;
    int kick = t - kickT_;
    if (cc < 0 && kick >= 0 && kick < 6) jx = (kick % 2) ? 2 : -2;
    if (k >= 0 && k < 6) jx = (k % 2) ? 2 : -2;
    else if (k >= 6 && k < 10) ang = ease((k - 5) / 4.0f) * 1.2f;
    else if (k >= 10 && k < 16) ang = 1.2f;
    else if (k >= 16 && k < 22) ang = 1.2f * (1 - ease((k - 15) / 6.0f));
    if (cc < THROW) {
        int p = cc >= 0 ? endPiece_ : piece;
        std::vector<std::pair<int, int>> cs; int bw, bh; pieceBox(p, cs, bw, bh);
        int rise = (cc < 0 && kick >= 0 && kick < 6) ? std::min(bh, kick * 4) : bh;
        int px0 = bx + jx - bw / 2, py0 = by - 25 - rise;
        for (auto& q : cs) {
            int yy = py0 + q.second * 6;
            if (yy + 6 <= by - 24) ToyBlocks::cell6(f, px0 + q.first * 6, yy, p);
        }
    }
    drawPot(f, bx + jx, by, ang);
    if (cc < WORM0 + 22) drawWorm(f, bx - 28 + jx, by - 26 - (k >= 0 && k < 6 ? 2 : 0), t, false);
    else if (cc < WORM0 + WORM_END) {
        int k = cc - WORM0 - 22;
        int wy = by - 26 + std::min(k, 18);
        int segs = std::max(0, 5 - k * 5 / 18);
        if (k < 20) drawWorm(f, bx - 6, wy, t, false, segs);
        drawPot(f, bx, by, 0);
    } else if (cc >= BEAR_WALK0 + 20 && cc < BEAR_TURN + 6) {
        drawWorm(f, bx + 4, by - 22 - std::min(6, (cc - BEAR_WALK0 - 20) / 2), t, true);
        drawPot(f, bx, by, 0);
    }
}
void KitchenRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    int cc = c();
    if (cc >= THROW && cc < THROW + THROW_LEN + 8) {
        std::vector<std::pair<int, int>> cs; int bw, bh; pieceBox(endPiece_, cs, bw, bh);
        float sx = 155 + 16, sy = 52 - 36, lx = 210, ly = 244 - bh;
        float u = std::min(1.0f, (float)(cc - THROW) / THROW_LEN), tt = std::sin(u * PI / 2) * 0.35f + u * 0.65f;
        float x = bez(sx, 196, lx, tt), y = bez(sy, -24, ly, tt);
        if (cc < THROW + THROW_LEN) for (auto& q : cs) ToyBlocks::cell6(f, rnd(x) + q.first * 6, rnd(y) + q.second * 6, endPiece_);
        else {
            int k = cc - THROW - THROW_LEN;
            for (int a = 0; a < 8; ++a) {
                float ang = a * PI / 4;
                f.set(rnd(lx + bw / 2.0f + std::cos(ang) * (3 + k * 2)), rnd(ly + bh + std::sin(ang) * (1 + k)), C(240, 230, 210));
            }
        }
    }
    for (auto& fl : flyers_) {
        float u = (t - fl.start) / 8.0f;
        if (u < 0 || u > 1) continue;
        for (int j = 0; j < 3; ++j) {
            float tt = clamp01(u * 1.15f - j * 0.06f), cx = (fl.sx[j] + fl.tx) / 2;
            float x = bez(fl.sx[j], cx, fl.tx, tt), y = bez(fl.sy[j], -30, 40, tt);
            int size = rnd(7 - 3 * tt);
            const RGB* r = ToyBlocks::ramp(fl.col[j]);
            int x0 = rnd(x) - size / 2, y0 = rnd(y) - size / 2;
            f.rect(x0, y0, size + 1, size + 1, C(r[1])); f.rect(x0 + 1, y0 + 1, size - 1, size - 1, C(r[2]));
        }
    }
    if (earnedT_ >= 0 && t <= earnedT_ + 18) {
        int k = t - earnedT_;
        for (int a = 0; a < 10; ++a) {
            float ang = a * PI / 5 + k * 0.1f, rr = 6.0f + k * 2;
            int x = rnd(BOWL_X + std::cos(ang) * rr * 1.5f), y = rnd(BOWL_Y - 2 + std::sin(ang) * rr * 0.6f);
            f.set(x, y, C(255, 250, 200)); f.set(x + 1, y, C(250, 220, 110));
        }
    }
    if (earnedT_ >= 0 && t - earnedT_ < 26) {
        int m = t - earnedT_;
        float u = clamp01((m - 4) / 10.0f);
        int ix = BOWL_X + rnd((34 - BOWL_X) * u), iy = BOWL_Y + rnd((236 - BOWL_Y) * u) - rnd(std::sin(u * PI) * 70);
        if (m >= 14) drawMemoryBook(f, 6, 226 - ((m < 18) ? (m % 2 ? 3 : 1) : 0));
        if (m < 15) {
            f.ellipse(ix, iy, 7, 4, C(OUT)); f.ellipse(ix, iy, 6, 3, C(150, 40, 40)); f.ellipse(ix, iy - 2, 5, 1, C(214, 232, 222));
        }
        if (m >= 14 && m < 24) {
            int r = (m - 14) * 3 + 6;
            for (int k = 0; k < 8; ++k) { float a = k * 0.785f; sparkle(f, 34 + rnd(std::cos(a) * r), 242 + rnd(std::sin(a) * r * 0.7f)); }
            if (m < 20) Font::center(f, "FISH SOUP!", 40, 212 - (m - 14), C(252, 228, 152), C(OUT));
        }
    }
    if (cc >= KID_SAY0 && cc < KID_SAY1) bubble(f, 318, 146, "MOM!", "ALL DONE!", 318, 176);
    if (cc >= MOM_SAY0 && cc < MOM_SAY1) bubble(f, MOM_X0 - 14, 86, "GOOD!", "LET'S GO!", MOM_X0, 112);
    if (cc >= FADE0) {
        float k = std::min(1.0f, (cc - FADE0 + 1) / 10.0f);
        for (auto& col : palette()) col = mix(col, RGB{ 0, 0, 0 }, k);
    }
}
void KitchenRound::reset() {
    flyers_.clear(); fish_.clear(); bites_.clear(); splashes_.clear(); splashX_.clear();
    lastT_ = 0; earnedT_ = -1; endStart_ = -1; endFrame_ = 0; kickT_ = -100; lastNext_ = -1; endPiece_ = 0;
}
void KitchenRound::specialEvent(int t) { if (endStart_ < 0) bites_.push_back(t); }
void KitchenRound::onLinesCleared(const std::vector<int>& rows, int, int t) {
    int k = 0;
    for (int r : rows) {
        Flyer fl;
        fl.start = t + k * 4;
        fl.tx = 386.0f + (t * 37 + k * 19) % 59;
        static const int XS[3] = { 1, 4, 8 };
        for (int j = 0; j < 3; ++j) {
            fl.sx[j] = (float)(boardX() + XS[j] * 8 + 4); fl.sy[j] = (float)(boardY() + r * 8 + 4);
            fl.col[j] = (r * 3 + j * 2 + t) % 7;
        }
        flyers_.push_back(fl);
        ++k;
    }
}
void KitchenRound::update(const PlayStateReader& g, int t) {
    lastT_ = t;
    if (g.next() != lastNext_) { if (lastNext_ >= 0) kickT_ = t; lastNext_ = g.next(); }
    for (auto it = flyers_.begin(); it != flyers_.end();) {
        if (t - it->start >= 8) {
            if (fish_.size() < 10) {
                fish_.push_back({ t, it->tx, it->col[1] });
                splashes_.push_back(t); splashX_.push_back(it->tx);
                int last = bites_.empty() ? -100 : bites_.back();
                bites_.push_back(std::max(t, last + 13));
                if (fish_.size() == 10) earnedT_ = t;
            }
            it = flyers_.erase(it);
        } else ++it;
    }
    if (earnedT_ >= 0 && endStart_ < 0 && t - earnedT_ >= 16) { endStart_ = t; endPiece_ = g.next(); }
    if (endStart_ >= 0) endFrame_ = t - endStart_;
}
