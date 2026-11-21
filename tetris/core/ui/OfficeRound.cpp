#include "ui/OfficeRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace Stage3Art;
static const int HERO_X = 286, FLOOR_Y = 216, DESK_X = 316;
static int rnd(float v) { return (int)std::lround(v); }
static void paycheck(Frame& f, int x, int y, int) {
    uint8_t ol = C(OLC);
    f.rect(x - 9, y - 6, 19, 13, ol); f.rect(x - 8, y - 5, 17, 11, C(244, 236, 214)); f.line(x - 8, y - 5, x, y + 1, C(200, 190, 166)); f.line(x + 8, y - 5, x, y + 1, C(200, 190, 166));
    f.rect(x - 2, y, 5, 4, C(120, 180, 100)); f.disc(x + 5, y + 3, 2, C(200, 52, 60));
}
static const Look COWORKER[3] = {
    { {44, 34, 44}, {70, 56, 66}, {140, 150, 170}, {110, 118, 140}, {60, 64, 80}, {44, 48, 60}, {80, 120, 200}, true, false, false, true, false, false },
    { {96, 60, 40}, {130, 86, 60}, {200, 130, 120}, {170, 100, 96}, {60, 64, 80}, {44, 48, 60}, {250, 250, 246}, false, true, true, false, false, false },
    { {30, 26, 30}, {60, 54, 60}, {90, 130, 110}, {66, 100, 84}, {60, 64, 80}, {44, 48, 60}, {200, 160, 60}, true, false, false, false, false, false } };
static const int CO_X[3] = { 232, 360, 446 };
void OfficeRound::reset() { queue_.clear(); cur_ = 0; curT_ = -1; last_ = 0; endT_ = -1; lines_ = 0; shown_ = 0; inbox_ = 3; done_ = 0; pileT_ = -100; }
void OfficeRound::onLinesCleared(const std::vector<int>&, int totalLines, int) { while (lines_ < totalLines && lines_ < 10) queue_.push_back(++lines_); }
void OfficeRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t; shown_ = cur_; ++done_; inbox_ = std::max(0, inbox_ - 1);
    if (cur_ % 3 == 0 && cur_ < 10) { pileT_ = t + DONE_LEN; inbox_ += 4; }
    if (cur_ == 10) endT_ = t + DONE_LEN;
}
void OfficeRound::update(const PlayStateReader&, int t) {
    last_ = t;
    if (endT_ >= 0) return;
    if (curT_ >= 0 && t - curT_ < DONE_LEN) return;
    if (t - pileT_ >= 0 && t - pileT_ < 12) return;
    curT_ = -1;
    if (!queue_.empty()) start(t);
}
void OfficeRound::drawWindow(Frame& f, int t) {
    float p = std::min(1.0f, shown_ / 9.0f);
    RGB top = p < 0.5f ? mix(RGB{ 250, 170, 100 }, RGB{ 120, 90, 160 }, p * 2) : mix(RGB{ 120, 90, 160 }, RGB{ 16, 20, 48 }, (p - 0.5f) * 2);
    RGB bot = p < 0.5f ? mix(RGB{ 254, 220, 150 }, RGB{ 220, 130, 140 }, p * 2) : mix(RGB{ 220, 130, 140 }, RGB{ 40, 46, 90 }, (p - 0.5f) * 2);
    int wx = 196, wy = 8, ww = 280, wh = 100;
    uint8_t ol = C(OLC), frame = C(200, 204, 212);
    f.rect(wx - 3, wy - 3, ww + 6, wh + 6, ol); f.gradient(wx, wy, ww, wh, { top, mix(top, bot, 0.5f), bot });
    if (p < 0.4f) { int sy = wy + 30 + rnd(p * 120); f.disc(wx + 210, sy, 12, C(mix(RGB{ 255, 230, 140 }, bot, 0.2f))); f.disc(wx + 210, sy, 9, C(255, 244, 196)); }
    if (p > 0.6f) { for (int k = 0; k < 24; ++k) f.set(wx + (k * 37) % ww, wy + 4 + (k * 13) % 40, C(230, 230, 250)); f.disc(wx + 40, wy + 18, 6, C(244, 240, 214)); f.disc(wx + 43, wy + 16, 5, C(mix(top, bot, 0.2f))); }
    static const int bh[14] = { 44, 62, 36, 70, 52, 30, 58, 76, 40, 66, 48, 34, 60, 50 };
    uint8_t city = C(mix(RGB{ 90, 96, 130 }, RGB{ 30, 34, 60 }, p)), cityD = C(mix(RGB{ 70, 74, 104 }, RGB{ 22, 24, 44 }, p)), win = p > 0.5f ? C(250, 214, 120) : C(170, 190, 220);
    for (int k = 0; k < 14; ++k) {
        int bx = wx + k * 20, tp = wy + wh - bh[k];
        f.rect(bx, tp, 20, bh[k], k % 2 ? city : cityD);
        for (int yy = tp + 4; yy < wy + wh - 3; yy += 6) for (int xx = bx + 3; xx < bx + 17; xx += 5) if (p < 0.5f || ((xx * 7 + yy * 3 + k) % 5) < 3) f.rect(xx, yy, 2, 3, win);
    }
    for (int k = 1; k < 4; ++k) f.rect(wx + k * ww / 4 - 1, wy, 3, wh, frame);
    f.rect(wx - 6, wy + wh + 2, ww + 12, 4, ol); f.rect(wx - 5, wy + wh + 3, ww + 10, 2, frame);
    for (int k = 0; k < 3; ++k) { int bx = (t / 4 + k * 90) % (ww + 20) + wx - 10; if (p < 0.6f && bx > wx && bx < wx + ww - 6) { f.set(bx, wy + 20 + k * 7, ol); f.set(bx - 1, wy + 19 + k * 7, ol); f.set(bx + 1, wy + 19 + k * 7, ol); } }
}
void OfficeRound::drawCubicles(Frame& f, int t) {
    uint8_t ol = C(OLC), wall = C(150, 156, 170), wallD = C(124, 130, 146), wallL = C(176, 182, 196), trim = C(90, 96, 110);
    for (int k = 0; k < 3; ++k) {
        int lv = k == 0 ? 2 : k == 1 ? 5 : 8, out = shown_ >= lv && endT_ < 0 ? t - (curT_ >= 0 ? curT_ : t) : 0;
        bool gone = shown_ > lv || (shown_ == lv && curT_ < 0) || endT_ >= 0;
        if (!gone) person(f, CO_X[k] + (shown_ == lv ? out * 3 : 0), 168, COWORKER[k], k == 1, shown_ == lv && out > 2 ? WALK : SIT, shown_ == lv && out > 2 ? REST : TYPE, CALM, t + k * 3);
        int mx = CO_X[k] + (k == 1 ? -26 : 24), face = k == 1 ? 1 : -1;
        f.rect(mx - 3, 106, 7, 24, ol); f.rect(mx - 2, 107, 5, 22, C(46, 50, 60));
        if (!gone) { f.vline(mx + face * 3, 108, 127, C(214, 240, 252)); for (int j = 0; j < 4; ++j) if ((t / 2 + j + k) % 3) f.set(mx + face * (4 + j % 2), 112 + j * 4, C(170, 214, 240)); }
    }
    f.rect(190, 128, 290, 44, ol); f.rect(190, 129, 290, 42, wall); f.hline(190, 479, 129, wallL); f.hline(190, 479, 170, wallD);
    for (int x = 196; x < 480; x += 8) for (int y = 134; y < 168; y += 8) f.set(x + (y / 8) % 2 * 4, y, wallD);
    for (int k = 0; k < 4; ++k) f.rect(190 + k * 96, 124, 4, 48, trim);
}
void OfficeRound::drawPapers(Frame& f, int x, int y, int n, bool messy) {
    uint8_t ol = C(OLC), paper = C(252, 250, 242), paperD = C(214, 210, 198);
    for (int k = 0; k < n; ++k) { int dx = messy ? ((k * 7) % 5) - 2 : 0, yy = y - k * 2; f.rect(x - 1 + dx, yy - 1, 18, 4, ol); f.rect(x + dx, yy, 16, 2, k % 2 ? paper : paperD); }
}
void OfficeRound::drawDesk(Frame& f, int t) {
    uint8_t ol = C(OLC), top = C(170, 124, 84), topL = C(200, 152, 106), front = C(140, 98, 66), frontD = C(110, 76, 52), mon = C(46, 50, 60), screen = C(150, 210, 240), screenL = C(214, 240, 252);
    int x0 = DESK_X, x1 = 470, y = 186;
    f.rect(x0 - 1, y - 1, x1 - x0 + 2, 7, ol); f.rect(x0, y, x1 - x0, 5, top); f.hline(x0, x1 - 1, y, topL);
    f.rect(x0 + 4, y + 5, 30, FLOOR_Y - y - 5, ol); f.rect(x0 + 5, y + 5, 28, FLOOR_Y - y - 6, front); for (int k = 0; k < 2; ++k) { f.hline(x0 + 7, x0 + 30, y + 12 + k * 10, frontD); f.rect(x0 + 16, y + 9 + k * 10, 6, 2, frontD); }
    f.rect(x1 - 10, y + 5, 6, FLOOR_Y - y - 5, ol); f.rect(x1 - 9, y + 5, 4, FLOOR_Y - y - 6, front);
    int mx = x0 + 34, my = y - 40;
    f.rect(mx - 22, y - 3, 20, 4, ol); f.rect(mx - 21, y - 2, 18, 2, C(220, 222, 228)); for (int k = 0; k < 4; ++k) f.set(mx - 19 + k * 4, y - 2, C(170, 172, 180));
    f.rect(mx - 1, my - 1, 8, 34, ol); f.rect(mx, my, 6, 32, mon); f.vline(mx, my + 1, my + 30, C(90, 96, 110));
    f.vline(mx - 1, my + 1, my + 30, screenL); f.vline(mx - 2, my + 3, my + 28, screen);
    for (int k = 0; k < 6; ++k) { int gy = my + 4 + k * 5; if ((t / 2 + k) % 4) f.set(mx - 4 - (k % 2), gy, screenL); }
    f.rect(mx + 1, my + 32, 4, 6, ol); f.rect(mx - 6, y - 2, 16, 3, ol); f.rect(mx - 5, y - 1, 14, 1, mon);
    (void)done_;
    int cx = x0 + 96;
    f.rect(cx, y - 11, 9, 11, ol); f.rect(cx + 1, y - 10, 7, 10, C(250, 250, 246)); f.rect(cx + 1, y - 10, 7, 2, C(120, 80, 50)); f.vline(cx + 9, y - 8, y - 4, ol); f.set(cx + 10, y - 6, ol);
    for (int k = 0; k < 3; ++k) { int sx = cx + 2 + k * 2, sy = y - 14 - (t + k * 3) % 6; f.set(sx + ((t / 3 + k) % 2), sy, C(230, 230, 236)); }
    int ix = x0 + 110, dx = x0 + 132;
    f.rect(ix - 2, y - 2, 22, 3, ol); f.rect(dx - 2, y - 2, 22, 3, ol);
    drawPapers(f, ix, y - 4, std::min(14, inbox_), inbox_ > 6);
    drawPapers(f, dx, y - 4, std::min(14, done_), false);
    Font::center(f, "IN", ix + 8, y + 8, C(250, 240, 210)); Font::center(f, "DONE", dx + 8, y + 8, C(250, 240, 210));
    uint8_t lampC = endT_ >= 0 && last_ - endT_ >= LAMP ? C(90, 90, 96) : C(255, 236, 160);
    f.rect(x1 - 18, y - 30, 3, 30, ol); f.poly({ {x1 - 26, y - 30}, {x1 - 8, y - 30}, {x1 - 12, y - 38}, {x1 - 22, y - 38} }, ol); f.poly({ {x1 - 25, y - 31}, {x1 - 9, y - 31}, {x1 - 13, y - 37}, {x1 - 21, y - 37} }, C(70, 120, 90)); f.hline(x1 - 24, x1 - 10, y - 30, lampC);
}
void OfficeRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    uint8_t wall = C(214, 210, 196), wallD = C(194, 190, 176), floor = C(96, 104, 120), floorD = C(82, 90, 104), floorL = C(112, 120, 136), ol = C(OLC);
    f.rect(0, 0, 480, FLOOR_Y, wall); for (int x = 0; x < 480; x += 20) f.vline(x, 0, FLOOR_Y, wallD);
    for (int k = 0; k < 5; ++k) { int lx = 30 + k * 100; f.rect(lx - 14, 0, 28, 4, ol); f.rect(lx - 13, 0, 26, 3, C(250, 252, 240)); }
    drawWindow(f, t);
    f.rect(0, FLOOR_Y, 480, 54, floor); f.hline(0, 479, FLOOR_Y, ol); f.hline(0, 479, FLOOR_Y + 1, floorL);
    for (int k = 0; k < 80; ++k) f.set((k * 61) % 480, FLOOR_Y + 4 + (k * 23) % 50, k % 2 ? floorD : floorL);
    int hour = 18 + std::min(6, shown_ * 6 / 10), cx = 20, cy = 52;
    f.disc(cx, cy, 13, ol); f.disc(cx, cy, 12, C(250, 248, 238)); for (int k = 0; k < 12; ++k) f.set(cx + rnd(std::cos(k * 0.5236f) * 9), cy + rnd(std::sin(k * 0.5236f) * 9), ol);
    float ah = (hour % 12) * 0.5236f - 1.5708f, am = (t % 60) * 0.1047f - 1.5708f;
    f.line(cx, cy, cx + rnd(std::cos(ah) * 5), cy + rnd(std::sin(ah) * 5), ol); f.line(cx, cy, cx + rnd(std::cos(am) * 8), cy + rnd(std::sin(am) * 8), C(200, 60, 60));
    Font::center(f, std::to_string(hour > 23 ? 12 : hour - 12) + (hour > 23 ? " AM" : " PM"), cx, cy + 16, C(80, 80, 96));
    drawCubicles(f, t);
    Stage2UI::pills(f, t);
    drawCommonUI(f);
}
void OfficeRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    uint8_t ol = C(OLC), chair = C(50, 50, 58), chairL = C(84, 84, 96);
    int e = endT_ >= 0 ? t - endT_ : -1;
    if (e >= LAMP) { drawDesk(f, t); return; }
    f.rect(HERO_X - 20, 158, 8, 40, ol); f.rect(HERO_X - 19, 159, 6, 38, chair); f.vline(HERO_X - 18, 160, 196, chairL);
    f.rect(HERO_X - 20, 194, 34, 5, ol); f.rect(HERO_X - 19, 195, 32, 3, chair);
    f.vline(HERO_X - 4, 199, FLOOR_Y - 3, C(150, 150, 160)); f.hline(HERO_X - 16, HERO_X + 8, FLOOR_Y - 2, C(150, 150, 160));
    bool typing = curT_ >= 0 && t - curT_ < DONE_LEN && e < 0;
    bool tired = shown_ >= 7 && e < 0 && !typing;
    if (e >= PAY + 26) { drawDesk(f, t); person(f, HERO_X + 10 + (e - PAY - 26) * 7, FLOOR_Y + 6, HERO_SUIT, false, WALK, REST, SMILE, t); return; }
    if (e >= STRETCH && e < PAY) person(f, HERO_X, FLOOR_Y, HERO_SUIT, false, SIT, OUT, SMILE, t);
    else if (e >= PAY) person(f, HERO_X, FLOOR_Y, HERO_SUIT, false, SIT, HOLD, SMILE, t);
    else person(f, HERO_X, FLOOR_Y, HERO_SUIT, false, SIT, TYPE, tired ? SLEEPY : CALM, typing ? t : t / 3);
    drawDesk(f, t);
    if (curT_ >= 0 && t - curT_ < DONE_LEN && e < 0) {
        float u = (t - curT_) / (float)DONE_LEN;
        int px = rnd(DESK_X + 110 + 22 * u), py = rnd(182 - inbox_ * 2 - std::sin(u * 3.1416f) * 20);
        f.rect(px - 1, py - 1, 18, 4, ol); f.rect(px, py, 16, 2, C(252, 250, 242));
    }
}
void OfficeRound::drawNext(Frame& f, int piece, int t) {
    uint8_t ol = C(OLC), cork = C(196, 150, 96), corkD = C(168, 122, 74), wood = C(120, 80, 52), note = C(252, 236, 140);
    int x = 196, y = 134;
    f.rect(x - 1, y - 1, 60, 50, ol); f.rect(x, y, 58, 48, wood); f.rect(x + 2, y + 2, 54, 44, cork);
    for (int k = 0; k < 24; ++k) f.set(x + 3 + (k * 17) % 52, y + 3 + (k * 11) % 42, corkD);
    f.rect(x + 8, y + 5, 42, 38, ol); f.rect(x + 9, y + 6, 40, 36, note); f.hline(x + 9, x + 48, y + 41, C(226, 206, 110)); f.disc(x + 29, y + 6, 1, C(220, 60, 60));
    Font::center(f, "NEXT", x + 29, y + 9, C(110, 90, 60));
    std::vector<std::pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    for (auto& c : cs) ToyBlocks::cell6(f, x + 29 - bw / 2 + (c.first - mx) * 6, y + 29 - bh / 2 + (c.second - my) * 6, piece);
    (void)t;
}
void OfficeRound::drawHud(Frame& f, int, int t) {
    uint8_t ol = C(OLC), paper = C(250, 250, 244), ink = C(70, 70, 90), done = C(70, 150, 80), todo = C(200, 196, 186);
    int x = 196, y = 184;
    f.rect(x - 1, y - 1, 60, 32, ol); f.rect(x, y, 58, 30, paper);
    Font::center(f, "TO DO", x + 29, y + 3, ink);
    for (int i = 0; i < 10; ++i) { int cx = x + 4 + i * 5; f.rect(cx, y + 12, 4, 4, ol); f.rect(cx + 1, y + 13, 2, 2, i < shown_ ? done : todo); }
    Font::center(f, std::to_string(shown_) + "/10", x + 29, y + 20, shown_ >= 10 ? done : ink);
    (void)t;
}
void OfficeRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    uint8_t ol = C(OLC);
    int bx = boardX(), by = boardY();
    if (t - pileT_ >= 0 && t - pileT_ < PILE_LEN && endT_ < 0) {
        int a = t - pileT_;
        if (a < 14 && (a / 3) % 2 == 0) Font::center(f, "OVERTIME!", bx + 40, by - 18, C(250, 120, 90), ol);
        for (int k = 0; k < 22; ++k) {
            int px = bx - 6 + (k * 29 + a * (k % 2 ? 2 : -1) + 400) % 90, py = by - 20 + a * (5 + k % 3) - (k * 37) % 80, w = 14 + k % 3 * 2;
            if (py < by - 10 || py > by + 160) continue;
            int tilt = (a + k) % 4 < 2 ? 1 : -1;
            f.poly({ {px, py}, {px + w, py + tilt}, {px + w, py + 9 + tilt}, {px, py + 9} }, ol); f.poly({ {px + 1, py + 1}, {px + w - 1, py + 1 + tilt}, {px + w - 1, py + 8 + tilt}, {px + 1, py + 8} }, C(252, 250, 242));
            f.hline(px + 3, px + w - 4, py + 4, C(170, 176, 196));
        }
    }
    if (curT_ >= 0 && t - curT_ < 6 && endT_ < 0) Font::center(f, "+1", DESK_X + 140, 150 - (t - curT_) * 2, C(70, 150, 80), ol);
    if (endT_ >= 0) {
        int e = t - endT_;
        if (e >= 0 && e < 8) Font::center(f, "MIDNIGHT", 330, 120, C(250, 236, 200), ol);
        if (e >= PAY - 8 && e < PAY) { paycheck(f, DESK_X + 70, 178, t); for (int k = 0; k < 4; ++k) star(f, DESK_X + 70 + ((k % 2) ? 12 : -12), 172 + (k / 2) * 10 - (t % 3)); }
        if (e >= PAY - 8 && e < PAY - 2) Font::center(f, "PAYDAY!", DESK_X + 70, 150, C(250, 220, 120), ol);
        flyToBook(f, e - PAY, DESK_X + 70, 178, paycheck, "PAYCHECK!", t);
        if (e >= LAMP) shade(f, 0, 0, 480, 270, std::min(3, 2 + (e - LAMP) / 3));
        if (e >= END - 12) { float k = std::min(1.0f, (e - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
}
