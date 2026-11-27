#include "ui/ClassroomRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include <algorithm>
#include <cmath>
using namespace std;

static const RGB OL{ 40, 30, 36 };
static const Sprite STUDENT("student at desk", {
"......HHHHHHHH......",
"....HHHHHHHHHHHH....",
"...HHHHHHHHHHHHHH...",
"..HHHHHHHHHHHHHHHH..",
"..HHHhHHHHHHHHhHHH..",
"..HHSSSSHHHHSSSSHH..",
"..HHSSSSSSSSSSSSHH..",
"..HSSSSSSSSSSSSSSH..",
"..HSSEESSSSSSEESSH..",
"..HSSEESSSSSSEESSH..",
"..sSSSSSSSSSSSSSSs..",
"..sSSppSSSSSSppSSs..",
"...SSSSSMSSMSSSSS...",
"....SSSSSMMSSSSS....",
"......sSSSSSSs......",
"........sSSs........",
"....BBWWWWWWWWBB....",
"..BBBBBWWRRWWBBBBB..",
".BBBBBBBWRRWBBBBBBB.",
".BBBBBBBBRRBBBBBBBB.",
"BBBBbBBBBRRBBBBbBBBB",
"BBBBbBBBBRRBBBBbBBBB",
"BBBBbBBBBBBBBBBbBBBB",
"BBBBbBBBBGBBBBBbBBBB",
"BBBBbBBBBBBBBBBbBBBB",
"BBBBbBBBBGBBBBBbBBBB",
}, { {'H', {168, 92, 44}}, {'h', {206, 128, 64}}, {'S', {246, 204, 164}}, {'s', {214, 164, 128}}, {'E', {34, 26, 34}},
     {'p', {244, 160, 150}}, {'M', {176, 80, 84}}, {'B', {44, 56, 96}}, {'b', {30, 38, 70}}, {'W', {244, 244, 240}},
     {'R', {190, 50, 60}}, {'G', {226, 190, 90}} });
static const Sprite STUDENT_BACK("student from behind", {
"......HHHHHHHH......",
"....HHHHHHHHHHHH....",
"...HHHHHHHHHHHHHH...",
"..HHHHHHHHHHHHHHHH..",
"..HHHhHHHHHHHHhHHH..",
"..HHHHHHHHHHHHHHHH..",
"..HHHHHHHHHHHHHHHH..",
"..HHHHHHHHHHHHHHHH..",
"..SHHHHHHHHHHHHHHS..",
"..SHHHHHHHHHHHHHHS..",
"..sHHHHHHHHHHHHHHs..",
"...HHHHHHHHHHHHHH...",
"....HHHHHHHHHHHH....",
".....hHHHHHHHHh.....",
"......sSSSSSSs......",
"........sSSs........",
"....BBWWWWWWWWBB....",
"..BBBBBBBBBBBBBBBB..",
".BBBBBBBBBBBBBBBBBB.",
".BBBBBBBBBBBBBBBBBB.",
"BBBBbBBBBBBBBBBbBBBB",
"BBBBbBBBBBBBBBBbBBBB",
"BBBBbBBBBbBBBBBbBBBB",
"BBBBbBBBBbBBBBBbBBBB",
"BBBBbBBBBbBBBBBbBBBB",
"BBBBbBBBBbBBBBBbBBBB",
}, { {'H', {168, 92, 44}}, {'h', {206, 128, 64}}, {'S', {246, 204, 164}}, {'s', {214, 164, 128}}, {'B', {44, 56, 96}}, {'b', {30, 38, 70}}, {'W', {244, 244, 240}} });
void ClassroomRound::reset() { lines_ = 0; clearT_ = -100; shown_ = -1; prev_ = -1; swapT_ = -100; bagT_ = -100; bag_ = false; give_ = false; used_ = false; saw_ = false; gifts_ = 0; gift_ = 0; endT_ = -1; last_ = 0; }
void ClassroomRound::onLinesCleared(const vector<int>&, int totalLines, int t) {
    if (totalLines / 3 > lines_ / 3 && totalLines < 10) { bag_ = true; bagT_ = t; give_ = true; used_ = false; saw_ = false; gift_ = gifts_ % 2 == 0 ? 0 : 1; ++gifts_; }
    lines_ = totalLines; clearT_ = t;
    if (totalLines >= 10 && endT_ < 0) endT_ = t + 4;
}
void ClassroomRound::update(const PlayStateReader& g, int t) {
    last_ = t;
    if (!bag_ || used_) return;
    if (saw_ && g.next() != gift_) { used_ = true; return; }
    if (g.next() == gift_) saw_ = true;
    if (t - bagT_ > 60) used_ = true;
}
void ClassroomRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    if (endT_ >= 0) flyToBook(f, t - endT_ - CLOSE, 406, 212, Stage2UI::notebookItem, "NOTEBOOK!", t);
}
void ClassroomRound::drawWindow(Frame& f, int t) {
    int x = 12, y = 36, w = 70, h = 74;
    f.rect(x - 3, y - 3, w + 6, h + 6, C(OL)); f.rect(x - 2, y - 2, w + 4, h + 4, C(236, 230, 214));
    f.gradient(x, y, w, h, { {150, 204, 240}, {176, 218, 244}, {206, 232, 246} });
    f.ellipse(x + 20, y + 18, 9, 4, C(250, 252, 252)); f.ellipse(x + 28, y + 16, 7, 4, C(250, 252, 252));
    for (int k = 0; k < 5; ++k) f.disc(x + 6 + k * 15, y + h - 6, 10 + (k % 2) * 3, C(k % 2 ? RGB{ 246, 170, 194 } : RGB{ 250, 196, 212 }));
    for (int k = 0; k < 6; ++k) { int px = x + (k * 13 + t) % w, py = y + 20 + (k * 17 + t * 2) % (h - 24); f.set(px, py, C(252, 214, 224)); }
    f.rect(x + w / 2 - 1, y, 3, h, C(236, 230, 214)); f.rect(x, y + h / 2 - 1, w, 3, C(236, 230, 214));
    f.rect(x - 6, y + h + 3, w + 12, 4, C(OL)); f.rect(x - 5, y + h + 3, w + 10, 2, C(244, 240, 228));
}
void ClassroomRound::drawBlackboard(Frame& f) {
    int x = 204, y = 20, w = 214, h = 78;
    f.rect(x + 3, y + 3, w, h, C(176, 166, 140));
    f.rect(x - 4, y - 4, w + 8, h + 8, C(OL)); f.rect(x - 3, y - 3, w + 6, h + 6, C(156, 104, 62)); f.hline(x - 3, x + w + 2, y - 3, C(206, 156, 100));
    f.rect(x, y, w, h, C(OL)); f.rect(x + 1, y + 1, w - 2, h - 2, C(46, 84, 64));
    for (int k = 0; k < 7; ++k) f.hline(x + 12 + k * 27, x + 26 + k * 27, y + 50 + (k % 3) * 7, C(58, 98, 76));
    uint8_t ch = C(232, 240, 232), chY = C(250, 226, 120), chP = C(246, 170, 190);
    Font::draw(f, "MARCH 2", x + 8, y + 7, ch);
    Font::center(f, "WELCOME!", x + w / 2, y + 22, chY);
    Font::center(f, "FIRST DAY OF SCHOOL", x + w / 2, y + 34, ch);
    for (int k = 0; k < 3; ++k) { int fx = x + 20 + k * 6, fy = y + 60; f.disc(fx, fy, 2, chP); }
    f.disc(x + w - 24, y + 62, 4, chY); for (int k = 0; k < 8; ++k) { float a = k * 0.785f; f.set(x + w - 24 + static_cast<int>(lround(cos(a) * 7)), y + 62 + static_cast<int>(lround(sin(a) * 7)), chY); }
    f.rect(x + 6, y + h + 4, w - 12, 4, C(OL)); f.rect(x + 7, y + h + 4, w - 14, 2, C(176, 124, 78));
    f.rect(x + 40, y + h + 2, 8, 2, C(246, 246, 246)); f.rect(x + 54, y + h + 2, 6, 2, C(250, 220, 120)); f.rect(x + w - 40, y + h + 1, 14, 3, C(120, 90, 70));
}
void ClassroomRound::drawCabinet(Frame& f) {
    int x = 226, y = 112, w = 170, h = 40;
    f.rect(x - 2, y - 4, w + 4, 5, C(OL)); f.rect(x - 1, y - 3, w + 2, 3, C(206, 156, 100));
    f.rect(x, y, w, h, C(OL)); f.rect(x + 1, y + 1, w - 2, h - 2, C(178, 124, 76));
    for (int k = 0; k < 4; ++k) {
        int dx = x + 4 + k * 41;
        f.rect(dx, y + 4, 38, h - 8, C(OL)); f.rect(dx + 1, y + 5, 36, h - 10, k == 0 ? C(60, 40, 34) : C(196, 140, 88));
        if (k != 0) { f.hline(dx + 1, dx + 36, y + 5, C(220, 168, 110)); f.rect(dx + 30, y + 18, 3, 4, C(232, 200, 110)); }
    }
    f.rect(x + 5, y + 5 + 18, 36, 2, C(120, 80, 54));
    f.rect(x + 46, y + 8, 10, 8, C(240, 236, 220)); f.rect(x + 47, y + 9, 8, 6, C(214, 90, 90));
    f.rect(x + 88, y + 8, 12, 10, C(90, 150, 210)); f.rect(x + 130, y + 8, 8, 12, C(250, 220, 120));
    f.rect(x + 76, y - 14, 16, 10, C(OL)); f.rect(x + 77, y - 13, 14, 8, C(120, 186, 96));
    f.disc(x + 84, y - 18, 5, C(86, 158, 72)); f.disc(x + 80, y - 20, 3, C(132, 194, 92));
    f.rect(x + w - 30, y - 16, 22, 12, C(OL)); f.rect(x + w - 29, y - 15, 20, 10, C(240, 236, 226)); f.disc(x + w - 19, y - 10, 3, C(OL));
}
static const int MX = 418;
static void desk(Frame& f, int cx, int y, int w, int d) {
    uint8_t ol = C(OL), top = C(214, 172, 114), topL = C(236, 200, 146), side = C(168, 120, 74), leg = C(110, 110, 120), seat = C(150, 106, 66), seatL = C(186, 136, 88);
    int lh = max(6, w / 3), x0 = cx - w / 2, x1 = cx + w / 2, k = d / 3;
    int cw = w / 2, ch = w / 3, cx0 = cx - cw / 2;
    f.rect(cx0 - 1, y - d - ch / 2 - 1, cw + 2, ch / 2 + 2, ol); f.rect(cx0, y - d - ch / 2, cw, ch / 2, seat); f.hline(cx0, cx0 + cw - 1, y - d - ch / 2, seatL);
    f.ellipse(cx, y + lh + 1, w / 2 + 2, max(1, w / 24), C(166, 120, 78));
    f.vline(x0 + 2, y, y + lh, ol); f.vline(x0 + 3, y, y + lh, leg); f.vline(x1 - 3, y, y + lh, ol); f.vline(x1 - 2, y, y + lh, leg);
    f.poly({ {x0 + k, y - d}, {x1 - k, y - d}, {x1, y + 1}, {x0, y + 1} }, ol);
    f.poly({ {x0 + k + 1, y - d + 1}, {x1 - k - 1, y - d + 1}, {x1 - 1, y}, {x0 + 1, y} }, top);
    f.hline(x0 + k + 1, x1 - k - 1, y - d + 1, topL);
    int sh = max(2, d / 3);
    f.rect(x0, y + 1, w + 1, sh + 1, ol); f.rect(x0 + 1, y + 1, w - 1, sh, side);
}
static int col(int i, float s) { static const int X[3] = { 262, 340, 418 }; return 340 + static_cast<int>(lround((X[i] - 340) * s)); }
void ClassroomRound::drawSideDesk(Frame& f) {
    for (int i = 0; i < 3; ++i) desk(f, col(i, 0.62f), 168, 38, 6);
    for (int i = 0; i < 3; ++i) desk(f, col(i, 0.8f), 188, 50, 9);
    for (int i = 0; i < 2; ++i) desk(f, col(i, 1.0f), 226, 66, 16);
}
void ClassroomRound::drawDesk(Frame& f) { desk(f, MX, 226, 66, 16); }
void ClassroomRound::drawSupplies(Frame& f) {
    uint8_t ol = C(OL);
    f.poly({ {393, 216}, {404, 216}, {403, 223}, {390, 223} }, ol); f.poly({ {394, 217}, {403, 217}, {402, 222}, {391, 222} }, C(226, 80, 80));
    f.poly({ {395, 213}, {405, 213}, {404, 216}, {393, 216} }, ol); f.poly({ {396, 214}, {404, 214}, {403, 215}, {395, 215} }, C(80, 140, 220));
    f.rect(432, 213, 14, 5, ol); f.rect(433, 214, 12, 3, C(250, 150, 190)); f.rect(435, 215, 2, 1, C(250, 230, 120));
    f.rect(434, 220, 16, 2, ol); f.rect(435, 220, 14, 1, C(240, 232, 160));
}
static void legs(Frame& f, int x, int y, int step) {
    uint8_t ol = C(OL), tr = C(34, 40, 70), sh = C(30, 24, 28);
    int a = step < 0 ? 0 : (step % 4 == 0 ? 2 : step % 4 == 2 ? -2 : 0);
    f.rect(x + 4, y, 12, 4, tr);
    f.rect(x + 4 + a, y + 3, 5, 9, ol); f.rect(x + 5 + a, y + 3, 3, 8, tr); f.rect(x + 4 + a, y + 11, 6, 2, sh);
    f.rect(x + 11 - a, y + 3, 5, 9, ol); f.rect(x + 12 - a, y + 3, 3, 8, tr); f.rect(x + 10 - a, y + 11, 6, 2, sh);
}
static void backpack(Frame& f, int x, int y, int n, int pop, bool open) {
    uint8_t ol = C(OL), d = C(36, 60, 124), m = C(62, 104, 186), l = C(104, 150, 226), hi = C(156, 196, 244), gold = C(236, 196, 96), goldD = C(176, 132, 52);
    f.line(x + 13, y + 2, x + 15, y - 4, ol); f.line(x + 15, y - 4, x + 23, y - 4, ol); f.line(x + 23, y - 4, x + 25, y + 2, ol);
    f.line(x + 14, y + 2, x + 16, y - 3, d); f.line(x + 24, y + 2, x + 22, y - 3, d);
    f.poly({ {x + 4, y}, {x + 34, y}, {x + 38, y + 4}, {x + 38, y + 30}, {x + 35, y + 33}, {x + 3, y + 33}, {x, y + 30}, {x, y + 4} }, ol);
    f.poly({ {x + 4, y + 1}, {x + 34, y + 1}, {x + 37, y + 4}, {x + 37, y + 30}, {x + 35, y + 32}, {x + 3, y + 32}, {x + 1, y + 30}, {x + 1, y + 4} }, m);
    f.vline(x + 2, y + 4, y + 29, l); f.hline(x + 5, x + 33, y + 2, l); f.vline(x + 36, y + 5, y + 30, d); f.hline(x + 4, x + 34, y + 31, d);
    f.rect(x + 1, y + 6, 3, 22, d); f.rect(x + 35, y + 6, 2, 22, d);
    if (open) {
        f.poly({ {x + 3, y + 1}, {x + 35, y + 1}, {x + 33, y - 3}, {x + 5, y - 3} }, ol);
        f.poly({ {x + 4, y}, {x + 34, y}, {x + 32, y - 2}, {x + 6, y - 2} }, C(24, 40, 86));
    } else {
        f.poly({ {x + 3, y + 1}, {x + 35, y + 1}, {x + 35, y + 9}, {x + 21, y + 12}, {x + 17, y + 12}, {x + 3, y + 9} }, ol);
        f.poly({ {x + 4, y + 2}, {x + 34, y + 2}, {x + 34, y + 8}, {x + 21, y + 11}, {x + 17, y + 11}, {x + 4, y + 8} }, d);
        f.hline(x + 5, x + 33, y + 3, m);
        f.rect(x + 16, y + 9, 7, 5, ol); f.rect(x + 17, y + 10, 5, 3, gold); f.hline(x + 17, x + 21, y + 12, goldD); f.set(x + 19, y + 11, ol);
    }
    f.poly({ {x + 5, y + 15}, {x + 33, y + 15}, {x + 34, y + 29}, {x + 4, y + 29} }, ol);
    f.poly({ {x + 6, y + 16}, {x + 32, y + 16}, {x + 33, y + 28}, {x + 5, y + 28} }, l);
    f.hline(x + 6, x + 32, y + 16, hi); f.hline(x + 6, x + 32, y + 18, d);
    f.rect(x + 29, y + 17, 2, 3, gold); f.set(x + 29, y + 20, goldD);
    for (int i = 0; i < 10; ++i) {
        int sx = x + 9 + (i % 5) * 5, sy = y + 22 + (i / 5) * 4, py = sy - (i == n - 1 && pop >= 0 && pop < 4 ? 2 : 0);
        if (i < n) { f.rect(sx - 1, py - 1, 3, 3, ol); f.set(sx, py, i % 2 ? C(250, 210, 90) : C(244, 120, 140)); f.set(sx - 1, py - 1, i % 2 ? C(255, 240, 170) : C(255, 190, 200)); }
        else f.set(sx, sy, m);
    }
}
static void sleeve(Frame& f, int sx, int sy, int hx, int hy) {
    f.taper(sx, sy, hx, hy, 6, 4, C(OL));
    f.taper(sx, sy, hx, hy, 4, 2, C(44, 56, 96));
    f.disc(hx, hy, 2, C(OL)); f.disc(hx, hy, 1, C(246, 204, 164));
}
static void packScaled(Frame& f, int cx, int y, float k) {
    static Frame tmp;
    uint8_t key = C(255, 0, 255);
    tmp.rect(0, 0, 44, 44, key);
    backpack(tmp, 3, 6, 10, -1, false);
    int w = static_cast<int>(lround(44 * k)), h = static_cast<int>(lround(44 * k));
    for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) {
        uint8_t c = tmp.get(static_cast<int>(i / k), static_cast<int>(j / k));
        if (c != key) f.set(cx - w / 2 + i, y - static_cast<int>(lround(6 * k)) + j, c);
    }
}
static float ease(float u) { u = max(0.0f, min(1.0f, u)); return u * u * (3 - 2 * u); }
void ClassroomRound::pose(int e, int& x, int& top, int& step, bool& back) const {
    float px = MX, py = 212;
    step = -1; back = false;
    if (e >= TO_BAG && e < GRAB) { float u = (e - TO_BAG) / static_cast<float>(GRAB - TO_BAG); px = MX + 48 * u; step = e; }
    else if (e >= GRAB && e < WEAR) px = MX + 48;
    else if (e >= WEAR) { float u = (e - WEAR) / static_cast<float>(TO_DOOR - WEAR); px = MX + 48 - 22 * u; py = 212 - 54 * u; step = e; back = true; }
    int rise = e < STAND1 ? (e - STAND0) * 2 : 12;
    if (e >= REACH - 2 && e < LIFT) rise -= 2;
    x = static_cast<int>(lround(px)) - 10;
    top = static_cast<int>(lround(py)) - 38 + (12 - rise) + (step >= 0 && step % 2 ? -1 : 0);
}
void ClassroomRound::drawCarry(Frame& f, int t) {
    int e = endT_ >= 0 ? t - endT_ : -1;
    if (e < STAND1 || e >= WEAR) return;
    int x, top, step; bool back;
    pose(e, x, top, step, back);
    int shx = x + 17, shy = top + 19, hx, hy;
    if (e < REACH) { float u = ease((e - STAND1 + 1) / static_cast<float>(REACH - STAND1)); hx = static_cast<int>(lround(shx + (MX - shx) * u)); hy = static_cast<int>(lround(shy + 6 + (232 - shy - 6) * u)); }
    else if (e < LIFT) { float u = ease((e - REACH) / static_cast<float>(LIFT - REACH)); hx = static_cast<int>(lround(MX + (shx + 6 - MX) * u)); hy = static_cast<int>(lround(232 + (shy + 4 - 232) * u - sin(u * 3.1416f) * 10)); }
    else { hx = shx + 6; hy = shy + 4 + ((e - LIFT) < 3 ? (e - LIFT) % 2 : 0); }
    int sw = e >= LIFT && e < TO_BAG ? (e - LIFT) % 2 * 2 - 1 : 0;
    if (e >= REACH) packScaled(f, hx + sw + 3, hy + 4, 1.0f - 0.42f * ease((e - REACH) / static_cast<float>(LIFT - REACH)));
    sleeve(f, shx, shy, hx, hy + 1);
    f.disc(hx, hy + 1, 2, C(OL)); f.disc(hx, hy + 1, 1, C(246, 204, 164));
}
void ClassroomRound::drawStudent(Frame& f, int t) {
    int e = endT_ >= 0 ? t - endT_ : -1;
    if (e >= STAND0) {
        if (e >= TO_DOOR) return;
        int x, top, step; bool back;
        pose(e, x, top, step, back);
        if (e >= STAND1) legs(f, x, top + 26, step);
        if (e >= WEAR && e < WEAR + 3) { blit(f, STUDENT_BACK, x, top); packScaled(f, x + 24 - (e - WEAR) * 5, top + 14, 0.58f); return; }
        blit(f, back ? STUDENT_BACK : STUDENT, x, top);
        if (back) {
            uint8_t ol = C(OL), st = C(30, 46, 96);
            f.line(x + 5, top + 16, x + 4, top + 22, ol); f.line(x + 14, top + 16, x + 15, top + 22, ol);
            packScaled(f, x + 10, top + 15, 0.58f);
            f.vline(x + 1, top + 18, top + 30, st); f.vline(x + 18, top + 18, top + 30, st);
        }
        return;
    }
    int a = t - clearT_;
    bool up = (a >= 0 && a < 10) || (e >= 0);
    int x = MX - 10, y = 180 + (up ? 0 : 2);
    blit(f, STUDENT, x, y);
    if (!up) { uint8_t sk = C(246, 204, 164), ey = C(34, 26, 34); f.hline(x + 5, x + 6, y + 8, sk); f.hline(x + 13, x + 14, y + 8, sk); f.hline(x + 5, x + 6, y + 9, ey); f.hline(x + 13, x + 14, y + 9, ey); }
}
void ClassroomRound::drawDoor(Frame& f, int t) {
    int e = endT_ >= 0 ? t - endT_ : -1;
    bool open = e >= TO_DOOR - 10 && e < DOOR_SHUT;
    uint8_t ol = C(OL);
    int x = 430, y = 62, w = 40, h = 94;
    f.rect(x - 3, y - 3, w + 6, h + 3, ol); f.rect(x - 2, y - 2, w + 4, h + 2, C(236, 230, 214));
    f.rect(x, y, w, h, ol);
    if (open) {
        f.rect(x + 1, y + 1, w - 2, h - 1, C(214, 214, 200)); f.rect(x + 1, y + h - 30, w - 2, 29, C(186, 176, 150));
        f.rect(x + 1, y + 1, 4, h - 1, C(30, 22, 26));
        f.poly({ {x + w - 1, y}, {x + w + 8, y + 6}, {x + w + 8, y + h + 4}, {x + w - 1, y + h} }, ol);
        f.poly({ {x + w, y + 2}, {x + w + 7, y + 7}, {x + w + 7, y + h + 2}, {x + w, y + h - 1} }, C(178, 124, 76));
    } else {
        f.rect(x + 1, y + 1, w - 2, h - 1, C(178, 124, 76)); f.hline(x + 1, x + w - 2, y + 1, C(206, 156, 100));
        f.rect(x + 7, y + 10, w - 14, 30, ol); f.rect(x + 8, y + 11, w - 16, 28, C(196, 224, 236)); f.hline(x + 9, x + w - 10, y + 13, C(236, 248, 252));
        f.rect(x + w - 8, y + 50, 4, 3, C(232, 200, 110));
    }
}
void ClassroomRound::drawBell(Frame& f, int t) {
    int e = endT_ >= 0 ? t - endT_ : -1;
    bool ring = e >= 0 && e < RING1;
    int x = 140, y = 22, j = ring ? ((t % 2) ? 1 : -1) : 0;
    uint8_t ol = C(OL);
    f.rect(x - 3, y - 6, 6, 4, ol);
    f.disc(x + j, y + 4, 9, ol); f.disc(x + j, y + 4, 8, C(206, 60, 60)); f.disc(x - 3 + j, y + 1, 3, C(240, 120, 110));
    f.rect(x - 10 + j, y + 4, 21, 6, C(218, 214, 196)); f.hline(x - 10 + j, x + 10 + j, y + 4, ol);
    f.disc(x + j, y + 6, 2, ol);
    if (ring) {
        for (int k = 0; k < 3; ++k) { f.line(x - 13 - k * 3, y - 2 + k * 4, x - 16 - k * 3, y - 4 + k * 4, ol); f.line(x + 13 + k * 3, y - 2 + k * 4, x + 16 + k * 3, y - 4 + k * 4, ol); }
        if ((t / 3) % 2 == 0) Font::draw(f, "RING!", x + 18, y - 2, C(206, 60, 60), C(252, 250, 240));
    }
}
void ClassroomRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    f.gradient(0, 0, 480, 156, { {232, 230, 210}, {226, 224, 204}, {218, 216, 194} });
    f.rect(0, 140, 480, 16, C(196, 176, 140)); f.hline(0, 479, 140, C(OL)); f.hline(0, 479, 141, C(220, 204, 170));
    f.rect(0, 156, 480, 114, C(190, 140, 92));
    for (int k = 0; k < 8; ++k) { int y = 158 + k * k * 2 + k * 4; f.hline(0, 479, y, C(166, 118, 76)); }
    for (int k = 0; k < 24; ++k) { int x0 = 240 + (k - 12) * 22, x1 = 240 + (k - 12) * 64; f.line(x0, 157, x1, 269, C(176, 128, 82)); }
    f.hline(0, 479, 156, C(OL));
    f.disc(452, 12, 8, C(OL)); f.disc(452, 12, 7, C(250, 250, 246)); f.line(452, 12, 452, 7, C(OL)); f.line(452, 12, 456, 13, C(OL));
    drawWindow(f, t);
    drawDoor(f, t);
    drawBell(f, t);
    drawBlackboard(f);
    drawCabinet(f);
    drawSideDesk(f);
    drawCommonUI(f);
}
void ClassroomRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    drawStudent(f, t);
    drawDesk(f);
    drawSupplies(f);
}
static void pieceAt(Frame& f, int piece, int cx, int by, int clipY) {
    vector<pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = min(mx, c.first); my = min(my, c.second); Mx = max(Mx, c.first); My = max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    for (auto& c : cs) { int yy = by - bh + (c.second - my) * 6; if (yy + 6 <= clipY) ToyBlocks::cell6(f, cx - bw / 2 + (c.first - mx) * 6, yy, piece); }
}
void ClassroomRound::drawNext(Frame& f, int piece, int t) {
    if (piece != shown_) {
        prev_ = shown_; shown_ = piece; swapT_ = t;
        if (bag_ && used_) bag_ = false;
    }
    piece_ = piece;
    int s = t - swapT_;
    float o = 1;
    int show = piece;
    if (bag_ || endT_ >= 0) { o = 0; }
    else if (prev_ >= 0 && s < 3) { o = 1 - (s + 1) / 3.0f; show = prev_; }
    else if (s < 5) { o = 0; }
    else if (s < 8) { o = (s - 4) / 3.0f; }
    if (o > 0.05f) pieceAt(f, show, 249, 135 - ((t / 6) % 2), 147);
    uint8_t ol = C(OL), door = C(196, 140, 88), doorL = C(220, 168, 110);
    int hinge = 267, fx = static_cast<int>(lround(hinge - 36 + 48 * o)), skew = static_cast<int>(lround(4 * o));
    int l = min(hinge, fx), r = max(hinge, fx);
    if (r - l >= 2) {
        int ly = fx < hinge ? 0 : skew;
        f.poly({ {l, 116 + ly}, {r, 116 + (fx < hinge ? 0 : -skew + skew)}, {r, 151}, {l, 151 - ly} }, ol);
        f.poly({ {l + 1, 117 + ly}, {r - 1, 117}, {r - 1, 150}, {l + 1, 150 - ly} }, door);
        f.vline(l + 1, 118 + ly, 148 - ly, doorL);
        if (r - l > 8) f.rect(fx < hinge ? l + 3 : r - 4, 130, 2, 4, C(232, 200, 110));
    }
    if (!bag_) Font::draw(f, "NEXT", 237, 108, C(250, 250, 240), ol);
}
void ClassroomRound::drawBagOnHook(Frame& f, int n, int t) {
    uint8_t ol = C(OL), ink = C(60, 70, 120), red = C(210, 70, 70);
    int bx = MX - 19, by = 236, ba = t - bagT_;
    f.rect(MX - 2, by - 6, 4, 3, C(110, 110, 120)); f.set(MX, by - 3, ol);
    if (bag_) pieceAt(f, gift_, MX, by + 10 - min(10, ba * 2) - ((t / 6) % 2), by);
    backpack(f, bx, by, n, t - clearT_, bag_);
    if (bag_ && ba > 4) Font::draw(f, "NEXT", bx - 26, by + 14, C(250, 250, 240), ol);
    f.line(bx + 38, by + 4, bx + 44, by + 10, ol);
    f.rect(bx + 40, by + 10, 31, 11, ol); f.rect(bx + 41, by + 11, 29, 9, C(252, 248, 230)); f.set(bx + 42, by + 12, ol);
    Font::center(f, to_string(n) + "/10", bx + 56, by + 13, n >= 10 ? red : ink);
}
void ClassroomRound::drawHud(Frame& f, int lines, int t) {
    Stage2UI::pills(f, t, state(), pillX(), pillY());
    int x = MX - 22, y = 211, w = 44, h = 14;
    uint8_t ol = C(OL);
    int n = min(max(lines, lines_), 10), a = t - clearT_, e = endT_ >= 0 ? t - endT_ : -1;
    if (e >= CLOSE) {
        if (e < STAND0) { f.poly({ {x + 2, y}, {x + w / 2 + 2, y}, {x + w / 2, y + h}, {x, y + h} }, ol); f.poly({ {x + 3, y + 1}, {x + w / 2 + 1, y + 1}, {x + w / 2 - 1, y + h - 1}, {x + 1, y + h - 1} }, C(70, 110, 190)); f.hline(x + 4, x + w / 2 - 2, y + 4, C(252, 250, 240)); }
        if (e < REACH) drawBagOnHook(f, n, t);
        drawCarry(f, t);
        if (e >= SIGN0) { float k = min(1.0f, (e - SIGN0 + 1) / 10.0f); for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k); }
        return;
    }
    uint8_t paper = C(252, 250, 240), rule = C(170, 196, 230), ink = C(60, 70, 120), red = C(210, 70, 70);
    f.poly({ {x + 2, y}, {x + w + 2, y}, {x + w, y + h}, {x, y + h} }, ol);
    f.poly({ {x + 3, y + 1}, {x + w / 2 + 1, y + 1}, {x + w / 2, y + h - 1}, {x + 1, y + h - 1} }, paper);
    f.poly({ {x + w / 2 + 2, y + 1}, {x + w + 1, y + 1}, {x + w - 1, y + h - 1}, {x + w / 2 + 1, y + h - 1} }, paper);
    f.vline(x + w / 2 + 1, y + 1, y + h - 1, C(200, 196, 180));
    bool ticking = a >= 0 && a < 10;
    int px = 0, py = 0;
    for (int i = 0; i < 10; ++i) {
        int c = i / 5, r = i % 5, lx = x + 3 + c * (w / 2 + 1), ly = y + 2 + r * 2 + r / 2;
        f.hline(lx + 4, lx + 18, ly + 1, rule);
        bool fresh = ticking && i == n - 1;
        if (i < n) f.hline(lx + 5, lx + 17, ly, ink);
        if (i < n && (!fresh || a >= 6)) { f.set(lx, ly, red); f.set(lx + 1, ly + 1, red); f.set(lx + 2, ly, red); f.set(lx + 3, ly - 1, red); }
        if (fresh) { px = lx + (a < 6 ? a / 2 : 3); py = ly + (a < 6 ? (a % 2) : -1); if (a >= 2) { f.set(lx, ly, red); f.set(lx + 1, ly + 1, red); } }
        if (!ticking && i == n && n < 10) { int p = (t / 2) % 10; for (int k = 0; k < p; ++k) f.set(lx + 5 + k, ly - (k % 2), ink); px = lx + 5 + p; py = ly; }
    }
    if (px == 0) { px = x + w - 6; py = y + h - 4; }
    bool right = px >= MX;
    sleeve(f, right ? MX + 7 : MX - 8, 199, px - 3, py + 4);
    f.line(px - 3, py + 4, px, py, ol); f.line(px - 4, py + 4, px - 1, py, ticking ? red : C(40, 60, 140));
    f.set(px, py, ticking ? red : ink);
    if (right) sleeve(f, MX - 8, 199, MX - 18, 218); else sleeve(f, MX + 7, 199, MX + 18, 218);
    drawBagOnHook(f, n, t);
}
