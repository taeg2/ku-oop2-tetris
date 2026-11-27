#include "ui/NurseryRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace std;

using namespace Stage3Art;
static const int FLOOR_Y = 228, DAD_X = 336, CRIB_X = 438;
static int rnd(float v) { return static_cast<int>(lround(v)); }
static void drawingItem(Frame& f, int x, int y, int) {
    uint8_t ol = C(OLC), paper = C(252, 250, 240);
    f.rect(x - 10, y - 8, 21, 17, ol); f.rect(x - 9, y - 7, 19, 15, paper);
    f.disc(x + 7, y - 5, 2, C(250, 200, 60));
    auto stick = [&](int px, int h, uint8_t c) { f.disc(px, y + 3 - h, 1, c); f.vline(px, y + 4 - h, y + 4, c); f.set(px - 1, y + 5 - h + 2, c); f.set(px + 1, y + 5 - h + 2, c); f.set(px - 1, y + 5, c); f.set(px + 1, y + 5, c); };
    stick(x - 6, 8, C(60, 90, 200)); stick(x - 1, 8, C(220, 70, 110)); stick(x + 4, 5, C(240, 150, 40));
    f.hline(x - 9, x + 9, y + 7, C(100, 180, 80));
}
static void babyBundle(Frame& f, int x, int y, bool cry, int t) {
    uint8_t ol = C(OLC), blanket = C(250, 210, 220), blanketD = C(226, 170, 186), skin = C(246, 204, 164), hair = C(120, 70, 46);
    f.ellipse(x, y + 2, 7, 11, ol); f.ellipse(x, y + 2, 6, 10, blanket); f.vline(x - 3, y - 4, y + 10, blanketD); f.line(x - 5, y + 6, x + 4, y - 2, blanketD);
    f.disc(x + 1, y - 11, 6, ol); f.disc(x + 1, y - 11, 5, skin); f.rect(x - 3, y - 17, 8, 3, hair); f.set(x - 3, y - 14, hair);
    f.set(x + 4, y - 12, ol); f.set(x + 5, y - 9, C(240, 160, 150));
    if (cry) { f.rect(x + 3, y - 9, 3, 2, ol); for (int k = 0; k < 2; ++k) f.set(x + 6 + k, y - 14 + (t + k) % 4, C(150, 200, 250)); }
    else { f.hline(x + 2, x + 4, y - 12, ol); f.set(x + 4, y - 8, C(176, 80, 84)); }
}
void NurseryRound::reset() { queue_.clear(); cur_ = 0; curT_ = -1; last_ = 0; endT_ = -1; lines_ = 0; shown_ = 0; cryT_ = -100; }
void NurseryRound::onLinesCleared(const vector<int>&, int totalLines, int) { while (lines_ < totalLines && lines_ < 10) queue_.push_back(++lines_); }
void NurseryRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t; shown_ = cur_;
    if (cur_ % 3 == 0 && cur_ < 10) cryT_ = t;
    if (cur_ == 10) endT_ = t;
}
void NurseryRound::update(const PlayStateReader&, int t) {
    last_ = t;
    if (endT_ >= 0) return;
    if (curT_ >= 0 && t - curT_ < STEP) return;
    if (t - cryT_ >= 0 && t - cryT_ < 10) return;
    curT_ = -1;
    if (!queue_.empty()) start(t);
}
void NurseryRound::drawRoom(Frame& f, int t) {
    uint8_t ol = C(OLC), wall = C(196, 222, 240), wallD = C(176, 204, 226), floor = C(210, 170, 120), floorD = C(186, 146, 100), rug = C(250, 214, 150), rugD = C(232, 186, 120);
    f.rect(0, 0, 480, FLOOR_Y, wall);
    for (int y = 10; y < FLOOR_Y - 30; y += 22) for (int x = (y / 22) % 2 * 18; x < 480; x += 36) { f.set(x, y, C(250, 240, 170)); f.set(x - 1, y, wallD); f.set(x + 1, y, wallD); f.set(x, y - 1, wallD); f.set(x, y + 1, wallD); }
    f.rect(0, FLOOR_Y - 26, 480, 3, C(250, 250, 246)); f.hline(0, 479, FLOOR_Y - 27, ol);
    f.rect(0, FLOOR_Y, 480, 270 - FLOOR_Y, floor); f.hline(0, 479, FLOOR_Y, ol);
    for (int y = FLOOR_Y + 8; y < 270; y += 9) f.hline(0, 479, y, floorD);
    f.ellipse(380, FLOOR_Y + 22, 80, 14, rugD); f.ellipse(380, FLOOR_Y + 21, 76, 12, rug); f.ellipse(380, FLOOR_Y + 21, 50, 7, rugD);
    int wx = 290, wy = 22, ww = 96, wh = 74, ph = phase();
    RGB a = ph == 0 ? RGB{ 16, 22, 56 } : ph == 1 ? RGB{ 250, 170, 140 } : RGB{ 130, 196, 246 }, b = ph == 0 ? RGB{ 40, 50, 100 } : ph == 1 ? RGB{ 254, 222, 170 } : RGB{ 210, 236, 252 };
    f.rect(wx - 4, wy - 4, ww + 8, wh + 8, ol); f.rect(wx - 3, wy - 3, ww + 6, wh + 6, C(250, 250, 246)); f.gradient(wx, wy, ww, wh, { a, b });
    if (ph == 0) { f.disc(wx + 70, wy + 20, 8, C(244, 240, 214)); f.disc(wx + 74, wy + 17, 7, C(a)); for (int k = 0; k < 10; ++k) f.set(wx + (k * 29) % ww, wy + 6 + (k * 17) % 40, C(230, 230, 250)); }
    else if (ph == 1) f.disc(wx + 20, wy + wh - 8, 10, C(255, 236, 170));
    else { f.disc(wx + 74, wy + 18, 9, C(255, 244, 180)); f.ellipse(wx + 30 + (t / 5) % 60, wy + 30, 12, 4, C(250, 252, 255)); }
    f.vline(wx + ww / 2, wy, wy + wh - 1, C(250, 250, 246)); f.hline(wx, wx + ww - 1, wy + wh / 2, C(250, 250, 246));
    f.rect(wx - 12, wy - 8, 10, wh + 24, C(250, 200, 214)); f.rect(wx + ww + 2, wy - 8, 10, wh + 24, C(250, 200, 214)); f.vline(wx - 8, wy - 8, wy + wh + 15, C(230, 170, 190)); f.vline(wx + ww + 6, wy - 8, wy + wh + 15, C(230, 170, 190));
    f.rect(wx - 16, wy - 10, ww + 32, 3, ol);
    int lx = 410, ly = 92;
    f.rect(lx - 1, ly, 20, 4, ol); f.rect(lx, ly - 18, 4, 18, ol); f.poly({ {lx - 6, ly - 18}, {lx + 10, ly - 18}, {lx + 6, ly - 28}, {lx - 2, ly - 28} }, ol);
    f.poly({ {lx - 5, ly - 19}, {lx + 9, ly - 19}, {lx + 5, ly - 27}, {lx - 1, ly - 27} }, C(255, 236, 160));
    if (ph == 0) for (int r = 8; r < 26; r += 4) for (int k = 0; k < 20; ++k) f.set(lx + 2 + rnd(cos(k * 0.314f) * r), ly - 20 + rnd(sin(k * 0.314f) * r * 0.8f), C(255, 236, 160));
    f.rect(lx - 6, ly + 4, 34, 6, ol); f.rect(lx - 5, ly + 5, 32, 4, C(240, 236, 226));
}
void NurseryRound::drawCrib(Frame& f, int t) {
    uint8_t ol = C(OLC), wood = C(250, 248, 240), woodD = C(214, 208, 196), sheet = C(200, 226, 246);
    int x0 = CRIB_X - 30, x1 = CRIB_X + 34, top = 156;
    f.rect(x0, top + 30, x1 - x0, 8, ol); f.rect(x0 + 1, top + 31, x1 - x0 - 2, 6, sheet);
    f.rect(x0 - 2, top, 5, FLOOR_Y - top, ol); f.rect(x1 - 3, top - 6, 5, FLOOR_Y - top + 6, ol);
    f.rect(x0 - 1, top, 3, FLOOR_Y - top, wood); f.rect(x1 - 2, top - 6, 3, FLOOR_Y - top + 6, wood);
    f.rect(x0, top, x1 - x0, 3, ol); f.rect(x0, top + 1, x1 - x0, 1, wood);
    for (int x = x0 + 6; x < x1 - 3; x += 6) { f.vline(x, top + 3, top + 30, ol); f.vline(x + 1, top + 3, top + 30, woodD); }
    f.rect(x0, top + 38, x1 - x0, 4, ol); f.rect(x0, top + 39, x1 - x0, 2, wood);
    int mx = CRIB_X, my = 112;
    f.vline(x1 - 1, my - 8, top - 6, ol); f.hline(mx, x1 - 1, my - 8, ol);
    for (int k = 0; k < 4; ++k) {
        float a = k * 1.57f + t * 0.12f; int ox = mx + rnd(cos(a) * 14), oy = my + 8 + rnd(sin(a) * 3);
        f.vline(ox, my - 8, oy - 3, C(150, 150, 160));
        static const RGB c[4] = { {250, 200, 90}, {150, 200, 250}, {250, 170, 190}, {170, 230, 170} };
        if (k % 2) { f.disc(ox, oy, 3, ol); f.disc(ox, oy, 2, C(c[k])); } else { f.poly({ {ox - 3, oy + 2}, {ox + 3, oy + 2}, {ox, oy - 3} }, ol); f.poly({ {ox - 2, oy + 1}, {ox + 2, oy + 1}, {ox, oy - 2} }, C(c[k])); }
    }
    f.hline(mx - 16, mx + 16, my - 8, ol);
}
void NurseryRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    drawRoom(f, t);
    drawCrib(f, t);
    if (endT_ < 0 && dark() > 0) shade(f, 0, 0, 480, 270, dark());
    Stage2UI::pills(f, t, state());
    drawCommonUI(f);
}
void NurseryRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    uint8_t ol = C(OLC);
    int e = endT_ >= 0 ? t - endT_ : -1, ph = phase();
    bool crying = t - cryT_ >= 0 && t - cryT_ < CRY && e < 0;
    bool busy = curT_ >= 0 && t - curT_ < STEP && e < 0;
    if (ph == 0 && e < 0) {
        int rock = rnd(sin(t * 0.3f) * 2);
        chair(f, DAD_X, FLOOR_Y, false, true, t);
        int hx = 0, hy = 0;
        person(f, DAD_X, FLOOR_Y - 2 + rock, HERO_HOME, false, SIT, HOLD, crying ? SLEEPY : busy ? SMILE : CALM, t, 2, &hx, &hy);
        babyBundle(f, hx - 2, hy - 2, crying, t);
        return;
    }
    if (ph == 1 && e < 0) {
        int a = busy ? t - curT_ : STEP, toss = busy ? rnd(sin(a * 3.1416f / STEP) * 46) : 0;
        int hx = 0, hy = 0;
        person(f, DAD_X + 10, FLOOR_Y, HERO_HOME, false, STAND, busy ? UP : HOLD, crying ? SLEEPY : SMILE, t, 2, &hx, &hy);
        int kx = hx + 4, ky = hy - 4 - toss;
        uint8_t skin = C(246, 204, 164), hair = C(120, 70, 46), dress = C(250, 200, 90), dressD = C(214, 160, 60), mouth = C(176, 80, 84);
        f.rect(kx - 6, ky + 14, 4, 9, ol); f.rect(kx + 2, ky + 14, 4, 9, ol); f.rect(kx - 5, ky + 14, 2, 8, skin); f.rect(kx + 3, ky + 14, 2, 8, skin);
        f.poly({ {kx - 7, ky - 1}, {kx + 7, ky - 1}, {kx + 10, ky + 16}, {kx - 10, ky + 16} }, ol); f.poly({ {kx - 6, ky}, {kx + 6, ky}, {kx + 9, ky + 15}, {kx - 9, ky + 15} }, dress); f.hline(kx - 8, kx + 8, ky + 13, dressD);
        int ay = busy ? -10 : 4;
        f.line(kx - 6, ky + 2, kx - 11, ky + ay, ol); f.line(kx + 6, ky + 2, kx + 11, ky + ay, ol); f.set(kx - 11, ky + ay, skin); f.set(kx + 11, ky + ay, skin);
        f.disc(kx, ky - 9, 9, ol); f.disc(kx, ky - 9, 8, skin); f.rect(kx - 7, ky - 18, 14, 5, hair); f.rect(kx - 8, ky - 15, 4, 8, hair); f.rect(kx + 5, ky - 15, 3, 3, hair);
        f.rect(kx - 4, ky - 10, 2, 2, ol); f.rect(kx + 3, ky - 10, 2, 2, ol); f.set(kx - 6, ky - 6, C(240, 160, 150)); f.set(kx + 6, ky - 6, C(240, 160, 150));
        if (crying) { f.rect(kx - 2, ky - 5, 4, 3, ol); } else { f.hline(kx - 2, kx + 2, ky - 5, mouth); f.set(kx - 3, ky - 6, mouth); f.set(kx + 3, ky - 6, mouth); }
        if (busy && a > 2 && a < STEP - 2) { star(f, hx - 6, hy - 40 - toss / 2); star(f, hx + 20, hy - 30 - toss / 2); }
        return;
    }
    int tx0 = 362, tx1 = 418, ty = FLOOR_Y - 20;
    chair(f, DAD_X, FLOOR_Y, false, false, t);
    chair(f, 438, FLOOR_Y, true, false, t);
    person(f, DAD_X, FLOOR_Y, HERO_HOME, false, SIT, e >= GIVE ? HOLD : REST, crying ? SLEEPY : SMILE, t);
    int gx = 438;
    person(f, gx, FLOOR_Y, GIRL, true, SIT, e >= 0 && e < GIVE ? OUT : TYPE, e >= 0 || busy ? SMILE : CALM, busy ? t : t / 3, 2);
    f.rect(tx0 - 1, ty - 1, tx1 - tx0 + 2, 5, ol); f.rect(tx0, ty, tx1 - tx0, 3, C(250, 196, 90)); f.rect(tx0 + 4, ty + 3, 4, FLOOR_Y - ty - 3, ol); f.rect(tx1 - 8, ty + 3, 4, FLOOR_Y - ty - 3, ol);
    f.rect(tx0 + 14, ty - 3, 20, 3, ol); f.rect(tx0 + 15, ty - 2, 18, 1, C(252, 250, 240));
    static const RGB cr[4] = { {220, 70, 70}, {70, 110, 220}, {250, 200, 60}, {90, 180, 90} };
    for (int k = 0; k < 4; ++k) f.rect(tx0 + 38 + k * 3, ty - 2, 2, 2, C(cr[k]));
}
void NurseryRound::drawNext(Frame& f, int piece, int t) {
    uint8_t ol = C(OLC), chest = C(240, 170, 120), chestD = C(206, 136, 92), lid = C(250, 196, 150);
    int x = 198, y = 186;
    f.rect(x - 1, y - 1, 66, 44, ol); f.rect(x, y, 64, 42, chest); f.rect(x, y, 64, 6, lid); f.hline(x, x + 63, y + 6, chestD);
    f.rect(x + 6, y + 10, 52, 28, C(252, 246, 232));
    static const RGB c[3] = { {120, 180, 240}, {250, 160, 180}, {170, 220, 140} };
    for (int k = 0; k < 3; ++k) { f.rect(x + 4 + k * 20, y - 9, 10, 10, ol); f.rect(x + 5 + k * 20, y - 8, 8, 8, C(c[k])); f.set(x + 8 + k * 20, y - 5, C(250, 250, 250)); }
    Font::center(f, "NEXT", x + 32, y + 12, C(150, 100, 80));
    vector<pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& cc : cs) { mx = min(mx, cc.first); my = min(my, cc.second); Mx = max(Mx, cc.first); My = max(My, cc.second); }
    int bw = (Mx - mx + 1) * 5, bh = (My - my + 1) * 5;
    for (auto& cc : cs) { int px = x + 32 - bw / 2 + (cc.first - mx) * 5, py = y + 28 - bh / 2 + (cc.second - my) * 5; const RGB* r = ToyBlocks::ramp(piece); f.rect(px, py, 5, 5, C(r[0])); f.rect(px + 1, py + 1, 3, 3, C(r[2])); f.set(px + 1, py + 1, C(r[4])); }
    (void)t; (void)chestD;
}
void NurseryRound::drawHud(Frame& f, int, int t) {
    uint8_t ol = C(OLC), ruler = C(250, 236, 180), mark = C(150, 120, 80), done = C(240, 110, 140);
    int x = 200, y0 = 40, y1 = 176;
    f.rect(x - 1, y0 - 1, 22, y1 - y0 + 2, ol); f.rect(x, y0, 20, y1 - y0, ruler);
    for (int i = 0; i <= 10; ++i) { int y = y1 - 4 - i * 12; f.hline(x, x + (i % 5 == 0 ? 10 : 6), y, mark); }
    int h = y1 - 4 - shown_ * 12;
    f.rect(x + 11, h - 1, 8, 3, ol); f.rect(x + 12, h, 6, 1, done); f.poly({ {x + 20, h - 3}, {x + 26, h}, {x + 20, h + 3} }, done);
    Font::center(f, "GROW", x + 10, y0 - 10, C(150, 120, 80));
    (void)t;
}
void NurseryRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    uint8_t ol = C(OLC);
    int e = endT_ >= 0 ? t - endT_ : -1;
    int bx = boardX(), by = boardY();
    if (curT_ >= 0 && t - curT_ < STEP && e < 0) {
        static const char* SAY[9] = { "SHHH...", "SLEEP TIGHT", "", "FIRST STEP!", "COME HERE", "", "WHAT IS IT?", "A HOUSE?", "" };
        if (shown_ >= 1 && shown_ <= 9 && SAY[shown_ - 1][0]) Font::center(f, SAY[shown_ - 1], DAD_X + 40, 116, C(250, 250, 240), ol);
    }
    if (t - cryT_ >= 0 && t - cryT_ < CRY && e < 0) {
        int a = t - cryT_;
        if (a < 12 && (a / 2) % 2 == 0) Font::center(f, "WAAAH!", CRIB_X - 20, 120, C(250, 120, 120), ol);
        if (a >= 6) {
            float lid = 0.5f + 0.5f * sin((a - 6) * 0.35f);
            int h = rnd(lid * lid * 135);
            f.rect(0, 0, 480, h, C(8, 8, 14)); f.rect(0, 270 - h, 480, h, C(8, 8, 14));
            if (h > 20) Font::center(f, "SO SLEEPY", 240, h - 14, C(140, 140, 170));
        }
    }
    if (e >= 0) {
        if (e < GIVE) { if (e > 6) Font::center(f, "FOR YOU, DADDY!", 400, 150, C(250, 120, 150), C(252, 250, 246)); }
        if (e >= GIVE - 6 && e < GIVE) drawingItem(f, 410, 176 - (e - GIVE + 6) * 2, t);
        flyToBook(f, e - GIVE, 400, 166, drawingItem, "HER DRAWING!", t);
        if (e >= GIVE + 26 && e < GIVE + 40) for (int k = 0; k < 3; ++k) { int hx = DAD_X + 6 + k * 10, hy = 150 - (e - GIVE - 26) * 2 - k * 5; uint8_t c = C(240, 90, 120); f.set(hx - 1, hy, c); f.set(hx + 1, hy, c); f.hline(hx - 2, hx + 2, hy + 1, c); f.hline(hx - 1, hx + 1, hy + 2, c); f.set(hx, hy + 3, c); }
        if (e >= END - 12) { float k = min(1.0f, (e - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
    (void)bx; (void)by;
}
