#include "ui/WeddingRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace std;

using namespace Stage3Art;
static const int FLOOR_Y = 236, GROOM_X = 344, BRIDE_X = 388, ARCH_X = 366;
static int rnd(float v) { return static_cast<int>(lround(v)); }
static void ringItem(Frame& f, int x, int y, int t) {
    uint8_t ol = C(OLC), gold = C(240, 200, 90), goldD = C(196, 150, 50), gem = C(220, 240, 255);
    f.disc(x, y + 2, 6, ol); f.disc(x, y + 2, 5, gold); f.disc(x, y + 2, 3, ol); f.disc(x, y + 2, 2, C(255, 0, 255));
    for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) if (f.get(x + dx, y + 2 + dy) == C(255, 0, 255)) f.set(x + dx, y + 2 + dy, ol);
    f.set(x + 3, y + 5, goldD); f.set(x - 4, y + 4, goldD);
    f.poly({ {x - 3, y - 4}, {x + 3, y - 4}, {x, y} }, ol); f.poly({ {x - 2, y - 4}, {x + 2, y - 4}, {x, y - 1} }, gem); f.rect(x - 2, y - 6, 5, 2, ol); f.set(x - 1, y - 5, gem);
    if ((t / 2) % 2) star(f, x + 4, y - 6);
}
static const Look GUEST[6] = {
    { {44, 34, 44}, {70, 56, 66}, {70, 90, 140}, {50, 66, 110}, {50, 54, 70}, {36, 40, 54}, {200, 60, 60}, true, false, false, false, false, false },
    { {120, 70, 46}, {160, 104, 70}, {240, 170, 180}, {210, 130, 150}, {60, 64, 80}, {44, 48, 60}, {250, 250, 246}, false, true, true, false, false, false },
    { {210, 210, 214}, {240, 240, 244}, {120, 100, 90}, {90, 74, 66}, {70, 70, 80}, {50, 50, 60}, {250, 250, 246}, false, false, false, true, true, false },
    { {30, 26, 30}, {60, 54, 60}, {160, 200, 230}, {120, 160, 200}, {60, 64, 80}, {44, 48, 60}, {250, 250, 246}, false, true, true, false, false, false },
    { {96, 60, 40}, {130, 86, 60}, {60, 120, 90}, {44, 92, 68}, {50, 54, 70}, {36, 40, 54}, {230, 190, 70}, true, false, false, false, false, false },
    { {150, 90, 60}, {190, 130, 90}, {250, 220, 120}, {214, 180, 90}, {60, 64, 80}, {44, 48, 60}, {250, 250, 246}, false, true, true, false, false, false } };
void WeddingRound::reset() { queue_.clear(); cur_ = 0; curT_ = -1; last_ = 0; endT_ = -1; lines_ = 0; shown_ = 0; gustT_ = -100; }
void WeddingRound::onLinesCleared(const vector<int>&, int totalLines, int) { while (lines_ < totalLines && lines_ < 10) queue_.push_back(++lines_); }
void WeddingRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t; shown_ = cur_;
    if (cur_ % 3 == 0 && cur_ < 10) gustT_ = t + 4;
    if (cur_ == 10) endT_ = t;
}
void WeddingRound::update(const PlayStateReader&, int t) {
    last_ = t;
    if (endT_ >= 0) return;
    if (curT_ >= 0 && t - curT_ < CHEER) return;
    curT_ = -1;
    if (!queue_.empty()) start(t);
}
void WeddingRound::drawGarden(Frame& f, int t) {
    uint8_t ol = C(OLC);
    f.gradient(0, 0, 480, 120, { {120, 180, 236}, {196, 222, 246}, {252, 226, 190} });
    f.disc(420, 34, 16, C(255, 240, 190)); f.disc(420, 34, 12, C(255, 250, 226));
    for (int k = 0; k < 3; ++k) { int cx = ((k * 160 + t / (4 + k)) % 600) - 60, cy = 20 + k * 10; f.ellipse(cx, cy, 20, 5, C(250, 252, 255)); f.ellipse(cx + 8, cy - 3, 10, 5, C(255, 255, 255)); }
    for (int x = 0; x < 480; ++x) { int h = 96 + rnd(sin(x * 0.03f) * 6 + sin(x * 0.011f + 1) * 8); f.vline(x, h, 130, C(150, 190, 140)); }
    for (int k = 0; k < 14; ++k) { int tx = 10 + k * 36, ty = 104 + (k % 3) * 3; f.disc(tx, ty, 12 + k % 3 * 2, C(90, 150, 90)); f.disc(tx - 3, ty - 3, 7, C(120, 180, 110)); }
    f.rect(0, 122, 480, 148, C(122, 184, 92));
    for (int k = 0; k < 90; ++k) { int x = (k * 53) % 480, y = 126 + (k * 31) % 140; f.vline(x, y, y + 2, C(98, 160, 74)); f.set(x + 1, y, C(98, 160, 74)); }
    for (int k = 0; k < 30; ++k) { int x = (k * 71) % 480, y = 130 + (k * 47) % 130; f.set(x, y, k % 2 ? C(255, 250, 250) : C(250, 200, 220)); f.set(x + 1, y, C(250, 230, 120)); }
    uint8_t run = C(250, 248, 242), runD = C(222, 218, 210);
    f.poly({ {ARCH_X - 14, 150}, {ARCH_X + 14, 150}, {ARCH_X + 34, 270}, {ARCH_X - 34, 270} }, run);
    f.line(ARCH_X - 14, 150, ARCH_X - 34, 269, runD); f.line(ARCH_X + 14, 150, ARCH_X + 34, 269, runD);
    for (int k = 0; k < 16; ++k) { float u = k / 15.0f; int y = 152 + rnd(u * 116), w = 14 + rnd(u * 20); f.set(ARCH_X - w + 2 + (k * 7) % 6, y, C(250, 190, 210)); f.set(ARCH_X + w - 4 - (k * 5) % 6, y + 2, C(240, 120, 150)); }
    (void)ol;
}
void WeddingRound::drawArch(Frame& f, int t) {
    uint8_t ol = C(OLC), wood = C(250, 248, 240), woodD = C(214, 208, 196), leaf = C(90, 160, 90), leafD = C(66, 128, 70);
    static const RGB flower[4] = { {250, 250, 250}, {250, 190, 210}, {240, 120, 150}, {250, 230, 140} };
    int x0 = ARCH_X - 44, x1 = ARCH_X + 44, top = 124;
    f.rect(x0 - 3, top, 7, FLOOR_Y - top, ol); f.rect(x0 - 2, top, 5, FLOOR_Y - top, wood); f.vline(x0 + 1, top, FLOOR_Y - 1, woodD);
    f.rect(x1 - 3, top, 7, FLOOR_Y - top, ol); f.rect(x1 - 2, top, 5, FLOOR_Y - top, wood); f.vline(x1 + 1, top, FLOOR_Y - 1, woodD);
    for (int a = 0; a <= 40; ++a) { float u = a / 40.0f * 3.1416f; int x = ARCH_X - rnd(cos(u) * 44), y = top - rnd(sin(u) * 30); f.disc(x, y, 4, ol); f.disc(x, y, 3, wood); }
    for (int a = 0; a <= 26; ++a) {
        float u = a / 26.0f * 3.1416f; int x = ARCH_X - rnd(cos(u) * 44), y = top - rnd(sin(u) * 30);
        f.disc(x + (a % 2 ? 2 : -2), y + 2, 4, a % 3 ? leaf : leafD);
        int fx = x + (a % 3) - 1, fy = y + (a % 2 ? -2 : 3), sw = (t / 6 + a) % 2;
        f.disc(fx, fy + sw, 3, ol); f.disc(fx, fy + sw, 2, C(flower[a % 4])); f.set(fx, fy + sw, C(250, 240, 160));
    }
    for (int k = 0; k < 8; ++k) { int y = top + 10 + k * 12; for (int s = 0; s < 2; ++s) { int x = s ? x1 : x0; f.disc(x + (k % 2 ? 3 : -3), y, 3, leafD); f.disc(x + (k % 2 ? -2 : 2), y + 5, 2, C(flower[(k + s) % 4])); } }
}
void WeddingRound::drawGuests(Frame& f, int t, bool front) {
    uint8_t ol = C(OLC), chair = C(250, 250, 246), chairD = C(214, 210, 200);
    bool cheer = (curT_ >= 0 && t - curT_ < CHEER) || (endT_ >= 0 && t - endT_ >= KISS);
    static const int GX[6] = { 222, 262, 296, 436, 466, 202 };
    for (int k = 0; k < 6; ++k) {
        bool fr = k >= 3 && k < 5;
        if (fr != front) continue;
        int x = GX[k], fl = fr ? 200 : 182;
        bool flip = x > ARCH_X;
        f.rect(x + (flip ? 4 : -14), fl - 30, 4, 30, ol); f.rect(x + (flip ? 5 : -13), fl - 29, 2, 28, chair);
        person(f, x, fl, GUEST[k], flip, SIT, cheer && (t + k) % 4 < 2 ? UP : REST, cheer ? SMILE : CALM, t + k * 2);
        f.rect(x - 14, fl - 10, 28, 3, ol); f.rect(x - 13, fl - 9, 26, 1, chairD);
    }
}
void WeddingRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    drawGarden(f, t);
    drawGuests(f, t, false);
    drawArch(f, t);
    Stage2UI::pills(f, t, state(), pillX(), pillY());
    drawCommonUI(f);
}
void WeddingRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    int e = endT_ >= 0 ? t - endT_ : -1;
    int bx = BRIDE_X + max(0, 3 - shown_) * 30;
    bool walking = curT_ >= 0 && t - curT_ < CHEER && shown_ <= 3 && e < 0;
    if (walking) bx += rnd((1.0f - (t - curT_) / static_cast<float>(CHEER)) * 30);
    bool vows = shown_ >= 4 && shown_ < 7 && curT_ >= 0 && t - curT_ < CHEER;
    Arm ga = e >= 0 && e < KISS ? OUT : REST, ba = e >= 0 && e < KISS ? OUT : HOLD;
    Mood gm = e >= RINGS ? SMILE : vows ? TALK : shown_ >= 4 ? SMILE : CALM, bm = e >= 0 || shown_ >= 4 ? SMILE : CALM;
    drawGuests(f, t, true);
    person(f, GROOM_X, FLOOR_Y, HERO_TUX, false, STAND, ga, gm, t);
    int hx = 0, hy = 0;
    person(f, bx, FLOOR_Y, BRIDE, true, walking ? WALK : STAND, ba, bm, t, 2, &hx, &hy);
    if (ba == HOLD) { uint8_t ol = C(OLC); f.disc(hx - 2, hy + 2, 5, ol); for (int k = 0; k < 5; ++k) f.disc(hx - 4 + (k % 3) * 3, hy + (k / 3) * 3, 2, k % 2 ? C(250, 190, 210) : C(250, 250, 250)); f.line(hx - 2, hy + 6, hx - 2, hy + 12, C(90, 160, 90)); }
    uint8_t veil = C(248, 248, 252);
    for (int k = 0; k < 12; ++k) { f.set(bx + 11 + (k % 3), FLOOR_Y - 76 + k * 3, veil); f.set(bx + 13 + (k % 2), FLOOR_Y - 74 + k * 3, C(226, 226, 236)); }
}
void WeddingRound::drawNext(Frame& f, int piece, int t) {
    uint8_t ol = C(OLC), wood = C(170, 120, 80), woodD = C(130, 90, 60), board = C(250, 246, 236);
    int x = 198, y = 136;
    f.line(x + 10, y + 50, x + 2, y + 92, ol); f.line(x + 50, y + 50, x + 58, y + 92, ol); f.line(x + 30, y + 50, x + 30, y + 90, woodD);
    f.rect(x - 1, y - 1, 62, 54, ol); f.rect(x, y, 60, 52, wood); f.rect(x + 3, y + 3, 54, 46, board);
    for (int k = 0; k < 6; ++k) f.disc(x + 4 + k * 10, y + 2, 2, k % 2 ? C(250, 190, 210) : C(250, 250, 250));
    Font::center(f, "NEXT", x + 30, y + 8, C(150, 90, 100));
    vector<pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = min(mx, c.first); my = min(my, c.second); Mx = max(Mx, c.first); My = max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    for (auto& c : cs) ToyBlocks::cell6(f, x + 30 - bw / 2 + (c.first - mx) * 6, y + 32 - bh / 2 + (c.second - my) * 6, piece);
    (void)woodD; (void)t;
}
void WeddingRound::drawHud(Frame& f, int, int t) {
    uint8_t ol = C(OLC), pink = C(240, 110, 140), pinkL = C(250, 190, 210), dim = C(220, 212, 206);
    int x = 198, y = 232;
    f.rect(x - 1, y - 1, 62, 30, ol); f.rect(x, y, 60, 28, C(250, 246, 236));
    Font::center(f, "WEDDING", x + 30, y + 3, C(150, 90, 100));
    for (int i = 0; i < 10; ++i) {
        int cx = x + 6 + (i % 5) * 12, cy = y + 13 + (i / 5) * 7;
        uint8_t c = i < shown_ ? pink : dim;
        f.set(cx - 1, cy, c); f.set(cx + 1, cy, c); f.hline(cx - 2, cx + 2, cy + 1, c); f.hline(cx - 1, cx + 1, cy + 2, c); f.set(cx, cy + 3, c);
        if (i < shown_) f.set(cx - 1, cy + 1, pinkL);
    }
    (void)t;
}
void WeddingRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    uint8_t ol = C(OLC);
    int bx = boardX(), by = boardY();
    if (curT_ >= 0 && t - curT_ < CHEER && endT_ < 0) {
        int a = t - curT_;
        for (int k = 0; k < 5; ++k) { int hx = 230 + k * 50 + (k % 2) * 8, hy = 150 - a * 3 - k * 4; uint8_t c = C(240, 110, 140); f.set(hx - 1, hy, c); f.set(hx + 1, hy, c); f.hline(hx - 2, hx + 2, hy + 1, c); f.hline(hx - 1, hx + 1, hy + 2, c); f.set(hx, hy + 3, c); }
        static const char* SAY[10] = { "AWW", "SO PRETTY", "HERE SHE COMES", "I DO", "I PROMISE", "FOREVER", "THE RINGS", "WITH THIS RING", "I DO TOO", "" };
        if (shown_ >= 1 && shown_ <= 9 && a < 8) Font::center(f, SAY[shown_ - 1], 366, 104, C(150, 90, 100), C(252, 250, 246));
    }
    if (t - gustT_ >= 0 && t - gustT_ < GUST && endT_ < 0) {
        int a = t - gustT_;
        float k = a < 6 ? a / 6.0f : a > 22 ? (GUST - a) / 8.0f : 1.0f;
        int n = static_cast<int>(240 * k);
        for (int i = 0; i < n; ++i) {
            int x = bx - 10 + (i * 37 + a * (5 + i % 3)) % 100, y = by + (i * 53 + rnd(sin((a + i) * 0.4f) * 5)) % 160;
            if (x < bx || x > bx + 78) continue;
            uint8_t c = i % 4 == 0 ? C(250, 250, 250) : i % 4 == 1 ? C(250, 190, 210) : C(240, 140, 170);
            f.set(x, y, c); f.set(x + 1, y, c); f.set(x, y + 1, C(214, 100, 130));
        }
        if (a < 10) Font::center(f, "PETALS", bx + 40, by - 18, C(250, 220, 230), ol);
    }
    if (endT_ >= 0) {
        int e = t - endT_;
        if (e < RINGS) { int cx = (GROOM_X + BRIDE_X) / 2, cy = FLOOR_Y - 50; for (int k = 0; k < 6; ++k) star(f, cx + rnd(cos(k * 1.05f + e * 0.3f) * (8 + e)), cy + rnd(sin(k * 1.05f + e * 0.3f) * (5 + e / 2))); if (e > 4) ringItem(f, cx, cy - 6, t); }
        flyToBook(f, e - RINGS, (GROOM_X + BRIDE_X) / 2, FLOOR_Y - 56, ringItem, "OUR RINGS!", t);
        if (e >= KISS) {
            int a = e - KISS;
            static const RGB cc[5] = { {240, 110, 140}, {250, 220, 120}, {150, 200, 250}, {250, 250, 250}, {170, 230, 170} };
            for (int k = 0; k < 60; ++k) { int cx = 200 + (k * 53 + rnd(sin((a + k) * 0.3f) * 6)) % 280, cy = -10 + (k * 29 + a * (2 + k % 3)) % 270; f.rect(cx, cy, 2, 1 + (a + k) % 2, C(cc[k % 5])); }
            for (int k = 0; k < 3; ++k) { int dx = ARCH_X - 10 + k * 14 + a * 4, dy = 120 - a * 3 + k * 6; if (dy < -6) continue; int w = (a + k) % 4 < 2 ? 3 : 1; f.rect(dx - 2, dy, 5, 3, ol); f.rect(dx - 1, dy + 1, 3, 1, C(252, 252, 252)); f.line(dx - 5, dy - w + 2, dx - 1, dy + 1, ol); f.line(dx + 5, dy - w + 2, dx + 1, dy + 1, ol); }
            if (a < 16) { Font::center(f, "JUST MARRIED", ARCH_X, 104, C(240, 110, 140), C(252, 250, 246)); for (int k = 0; k < 3; ++k) { int hx = GROOM_X + 14 + k * 8, hy = FLOOR_Y - 80 - a * 2 - k * 4; uint8_t c = C(240, 90, 120); f.set(hx - 1, hy, c); f.set(hx + 1, hy, c); f.hline(hx - 2, hx + 2, hy + 1, c); f.hline(hx - 1, hx + 1, hy + 2, c); f.set(hx, hy + 3, c); } }
        }
        if (e >= END - 12) { float k = min(1.0f, (e - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
}
