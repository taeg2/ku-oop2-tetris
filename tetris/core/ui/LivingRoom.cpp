#include "ui/LivingRoom.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace Stage3Art;
static const int FLOOR_Y = 222;
static int rnd(float v) { return static_cast<int>(std::lround(v)); }
static void clock(Frame& f, int cx, int cy, int spd, int t) {
    uint8_t ol = C(OLC);
    f.disc(cx, cy, 13, ol); f.disc(cx, cy, 12, C(250, 248, 238)); f.disc(cx, cy, 10, C(244, 240, 228));
    for (int k = 0; k < 4; ++k) f.set(cx + rnd(std::cos(k * 1.5708f) * 10), cy + rnd(std::sin(k * 1.5708f) * 10), ol);
    float am = t * 0.1f * spd, ah = am / 12;
    f.line(cx, cy, cx + rnd(std::cos(ah) * 5), cy + rnd(std::sin(ah) * 5), ol); f.line(cx, cy, cx + rnd(std::cos(am) * 9), cy + rnd(std::sin(am) * 9), C(200, 60, 60));
}
static RGB dk(RGB c, float k) { return mix(c, RGB{ 40, 20, 24 }, k); }
static RGB lt(RGB c, float k) { return mix(c, RGB{ 255, 246, 220 }, k); }
static void bx(Frame& f, int x, int y, int w, int h, RGB c) { f.rect(x, y, w, h, C(OLC)); f.rect(x + 1, y + 1, w - 2, h - 2, C(c)); f.hline(x + 1, x + w - 2, y + 1, C(lt(c, 0.25f))); }
void LivingRoom::draw(Frame& f, int t, int SOFA_X, int DOOR_X, bool tvStatic, int clockSpeed) {
    uint8_t ol = C(OLC);
    const RGB paper{ 244, 228, 174 }, panel{ 230, 226, 220 }, plank{ 214, 150, 84 }, white{ 236, 230, 220 }, stone{ 206, 200, 196 };
    int wain = FLOOR_Y - 34;
    f.rect(0, 0, 480, wain, C(paper));
    for (int y = 6; y < wain; y += 12) for (int x = (y / 12 % 2) * 12; x < 480; x += 24) f.set(x, y, C(dk(paper, 0.06f)));
    f.rect(0, wain, 480, FLOOR_Y - wain, C(panel)); f.hline(0, 479, wain, ol); f.hline(0, 479, wain + 1, C(lt(panel, 0.4f))); f.rect(0, wain + 2, 480, 2, C(dk(panel, 0.08f)));
    for (int x = 4; x < 480; x += 40) { f.rect(x, wain + 7, 32, FLOOR_Y - wain - 12, C(dk(panel, 0.1f))); f.rect(x + 1, wain + 8, 30, FLOOR_Y - wain - 14, C(panel)); f.hline(x + 1, x + 30, wain + 8, C(lt(panel, 0.5f))); }
    f.rect(0, FLOOR_Y - 3, 480, 3, C(dk(panel, 0.15f)));
    f.rect(0, FLOOR_Y, 480, 270 - FLOOR_Y, C(plank)); f.hline(0, 479, FLOOR_Y, ol);
    for (int r = 0; FLOOR_Y + 1 + r * 8 < 270; ++r) { int y = FLOOR_Y + 1 + r * 8; f.hline(0, 479, y + 7, C(dk(plank, 0.25f))); f.hline(0, 479, y, C(lt(plank, 0.12f))); for (int x = (r * 37) % 68; x < 480; x += 68) f.vline(x, y, y + 6, C(dk(plank, 0.25f))); for (int x = (r * 53 + 20) % 90; x < 480; x += 90) f.hline(x, x + 6, y + 3, C(dk(plank, 0.08f))); }
    int fx = 280, fy = FLOOR_Y - 2;
    for (int y = fy - 4; y < 270; ++y) for (int x = fx - 46; x < fx + 46; ++x) { int d = std::abs(x - fx) + (y - fy) * 3; if (d < 40 + ((t / 2) % 3) * 2 && (x + y) % 2 == 0) f.set(x, y, C(lt(plank, 0.35f))); }
    f.rect(fx - 4, 0, 9, fy - 62, ol); f.rect(fx - 3, 0, 7, fy - 62, C(110, 110, 116)); f.vline(fx - 2, 0, fy - 63, C(150, 150, 156)); for (int y = 20; y < fy - 66; y += 30) f.hline(fx - 4, fx + 4, y, ol);
    f.disc(fx, fy - 34, 26, ol); f.disc(fx, fy - 34, 25, C(stone)); f.rect(fx - 26, fy - 34, 53, 35, ol); f.rect(fx - 25, fy - 34, 51, 34, C(stone));
    for (int k = 0; k < 14; ++k) { int sx = fx - 20 + (k * 29) % 40, sy = fy - 50 + (k * 17) % 46; if ((sx - fx) * (sx - fx) + (sy - fy + 34) * (sy - fy + 34) < 22 * 22 || sy > fy - 34) { f.hline(sx, sx + 3, sy, C(dk(stone, 0.15f))); f.set(sx, sy - 1, C(lt(stone, 0.3f))); } }
    f.disc(fx, fy - 26, 14, ol); f.rect(fx - 14, fy - 26, 29, 25, ol); f.disc(fx, fy - 26, 13, C(50, 30, 30)); f.rect(fx - 13, fy - 26, 27, 25, C(50, 30, 30));
    int fl = (t / 2) % 3;
    f.poly({ {fx - 9, fy - 3}, {fx + 9, fy - 3}, {fx + 5, fy - 16 + fl}, {fx + 1, fy - 24 + fl}, {fx - 3, fy - 15 - fl}, {fx - 6, fy - 12} }, C(240, 140, 50));
    f.poly({ {fx - 5, fy - 3}, {fx + 5, fy - 3}, {fx + 3, fy - 11 - fl}, {fx, fy - 15 + fl}, {fx - 3, fy - 10} }, C(255, 220, 110));
    f.rect(fx - 12, fy - 4, 25, 3, C(90, 60, 40)); f.rect(fx - 30, fy, 61, 3, ol); f.rect(fx - 29, fy, 59, 2, C(dk(stone, 0.1f)));
    for (int k = 0; k < 3; ++k) { int a = (t * 2 + k * 9) % 26; f.set(fx - 4 + k * 4 + (a / 5) % 2, fy - 26 - a, C(250, 200, 120)); }
    int bkx = 218, bky = 120, bkw = 34;
    bx(f, bkx, bky, bkw, FLOOR_Y - bky, white);
    static const RGB BOOK[6] = { {190, 70, 70}, {70, 110, 170}, {90, 150, 90}, {220, 180, 80}, {150, 100, 160}, {230, 130, 70} };
    for (int r = 0; r < 4; ++r) { int sy = bky + 4 + r * 25, x = bkx + 3; for (int k = 0; x < bkx + bkw - 4; ++k) { int w = 2 + (k * 7 + r) % 3, h = 15 + (k * 5 + r * 3) % 6; if ((k + r) % 7 == 5) { f.line(x, sy + 20, x + 5, sy + 20 - h + 2, ol); x += 6; continue; } f.rect(x, sy + 21 - h, w, h, C(BOOK[(k + r * 2) % 6])); f.vline(x + w - 1, sy + 21 - h, sy + 20, C(dk(BOOK[(k + r * 2) % 6], 0.3f))); x += w + 1; } f.rect(bkx + 1, sy + 21, bkw - 2, 3, C(dk(white, 0.12f))); f.hline(bkx + 1, bkx + bkw - 2, sy + 21, ol); }
    f.rect(bkx + 8, bky - 14, 16, 14, ol); f.rect(bkx + 9, bky - 13, 14, 12, C(190, 110, 70)); f.hline(bkx + 9, bkx + 22, bky - 10, C(160, 90, 56));
    f.disc(bkx + 16, bky - 24, 12, ol); f.disc(bkx + 16, bky - 24, 11, C(80, 150, 70)); f.disc(bkx + 13, bky - 27, 6, C(110, 180, 90)); for (int k = 0; k < 6; ++k) f.set(bkx + 10 + (k * 5) % 13, bky - 30 + (k * 7) % 14, C(60, 120, 56));
    int pnx = 182, pny = 62;
    f.rect(pnx - 1, pny - 1, 46, 26, ol); f.rect(pnx, pny, 44, 24, C(250, 180, 90)); f.rect(pnx, pny, 44, 8, C(240, 140, 90)); f.rect(pnx, pny + 13, 44, 11, C(70, 120, 170)); f.disc(pnx + 22, pny + 12, 6, C(255, 236, 140)); f.hline(pnx, pnx + 43, pny + 13, C(200, 120, 70)); f.hline(pnx + 4, pnx + 12, pny + 17, C(120, 170, 210)); f.hline(pnx + 26, pnx + 38, pny + 20, C(120, 170, 210));
    int ddx = 198;
    f.rect(ddx - 14, 112, 29, FLOOR_Y - 112, ol); f.rect(ddx - 13, 113, 27, FLOOR_Y - 113, C(150, 96, 60)); f.rect(ddx - 10, 117, 21, 40, C(126, 78, 48)); f.rect(ddx - 10, 162, 21, 54, C(126, 78, 48)); f.rect(ddx + 8, 164, 3, 4, C(232, 200, 110)); f.rect(ddx - 16, FLOOR_Y - 2, 33, 2, C(110, 70, 44));
    clock(f, 246, 34, clockSpeed, t);
    f.rect(SOFA_X - 12, 52, 34, 28, ol); f.rect(SOFA_X - 11, 53, 32, 26, C(200, 150, 90)); f.rect(SOFA_X - 8, 56, 26, 20, C(190, 220, 246)); f.rect(SOFA_X - 8, 70, 26, 6, C(130, 190, 100)); f.disc(SOFA_X - 1, 66, 2, C(168, 92, 44)); f.disc(SOFA_X + 5, 66, 2, C(120, 70, 46)); f.disc(SOFA_X + 10, 68, 1, C(120, 70, 46)); f.rect(SOFA_X - 2, 63, 3, 7, C(120, 150, 110)); f.rect(SOFA_X + 4, 63, 3, 7, C(214, 120, 140));
    f.ellipse(SOFA_X + 20, FLOOR_Y + 14, 74, 9, C(dk(plank, 0.2f))); f.ellipse(SOFA_X + 20, FLOOR_Y + 13, 74, 9, C(222, 214, 204)); f.ellipse(SOFA_X + 20, FLOOR_Y + 13, 56, 6, C(232, 226, 218));
    int lx = SOFA_X - 30;
    f.line(lx, FLOOR_Y - 34, lx - 7, FLOOR_Y, ol); f.line(lx, FLOOR_Y - 34, lx + 7, FLOOR_Y, ol); f.line(lx, FLOOR_Y - 34, lx, FLOOR_Y, ol);
    f.vline(lx, 132, FLOOR_Y - 34, ol); f.vline(lx + 1, 132, FLOOR_Y - 34, C(120, 80, 50));
    bx(f, lx - 10, 116, 22, 17, { 244, 228, 170 }); for (int k = 0; k < 9; ++k) f.vline(lx - 8 + k * 2, 122, 131, C(220, 196, 140));
    for (int y = 133; y < 150; ++y) { int hw = 9 + (y - 133); for (int x = lx - hw; x <= lx + hw; ++x) if ((x + y) % 2 == 0 && f.get(x, y) == C(paper)) f.set(x, y, C(lt(paper, 0.5f))); }
    int tvx = 438;
    bx(f, tvx - 20, FLOOR_Y - 26, 40, 22, white); f.rect(tvx - 16, FLOOR_Y - 18, 32, 8, C(200, 196, 190)); f.rect(tvx - 4, FLOOR_Y - 15, 8, 2, ol); f.rect(tvx - 18, FLOOR_Y - 4, 3, 4, ol); f.rect(tvx + 15, FLOOR_Y - 4, 3, 4, ol);
    bx(f, tvx - 8, FLOOR_Y - 30, 16, 5, { 160, 160, 170 });
    bx(f, tvx - 22, FLOOR_Y - 62, 44, 33, { 40, 44, 60 });
    int sx = tvx - 20, sy = FLOOR_Y - 60, sw = 40, sh = 29;
    if (tvStatic) { for (int y = sy; y < sy + sh; ++y) for (int x = sx; x < sx + sw; ++x) { unsigned h = (static_cast<unsigned>(x) * 2654435761u) ^ (static_cast<unsigned>(y) * 40503u) ^ (static_cast<unsigned>(t) * 97u); f.set(x, y, h % 3 == 0 ? C(240, 240, 240) : h % 3 == 1 ? C(130, 130, 136) : C(40, 40, 44)); } }
    else { f.rect(sx, sy, sw, sh, C(50, 120, 170)); f.line(sx + 2, sy + 2, sx + 14, sy + 14, C(110, 170, 210)); f.rect(sx + 2, sy + 21, 16, 3, C(220, 120, 120)); int bob = (t / 5) % 4; f.ellipse(sx + 28, sy + 12 + (bob == 1), 4, 5, C(240, 200, 170)); f.rect(sx + 24, sy + 17, 9, 8, C(250, 220, 120)); }
    f.rect(DOOR_X - 12, 104, 28, FLOOR_Y - 104, ol); f.rect(DOOR_X - 10, 106, 24, FLOOR_Y - 106, C(14, 10, 14)); f.rect(DOOR_X - 12, 104, 28, 2, C(150, 140, 136)); f.vline(DOOR_X - 12, 104, FLOOR_Y - 1, C(150, 140, 136)); f.vline(DOOR_X + 15, 104, FLOOR_Y - 1, C(150, 140, 136));
    int vx = 404;
    f.rect(vx - 12, FLOOR_Y - 16, 25, 4, ol); f.rect(vx - 11, FLOOR_Y - 15, 23, 2, C(244, 240, 232)); f.rect(vx - 10, FLOOR_Y - 12, 2, 12, ol); f.rect(vx + 9, FLOOR_Y - 12, 2, 12, ol);
    f.rect(vx - 2, FLOOR_Y - 24, 5, 8, ol); f.rect(vx - 1, FLOOR_Y - 23, 3, 7, C(60, 150, 170));
    for (int k = 0; k < 5; ++k) { int ex = vx - 6 + k * 3, ey = FLOOR_Y - 34 + (k % 2) * 3 + rnd(std::sin(t * 0.1f + k) * 0.6f); f.line(vx, FLOOR_Y - 24, ex, ey + 2, C(70, 130, 70)); f.set(ex, ey, C(250, 250, 244)); f.set(ex + 1, ey, C(250, 250, 244)); f.set(ex, ey + 1, C(250, 230, 120)); }
}
