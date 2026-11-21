#include "ui/UniRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include <algorithm>
#include <cmath>
static const RGB OL{ 40, 30, 36 };
static const int SIT_X = 380, SIT_Y = 238;
static int rnd(float v) { return (int)std::lround(v); }
static const Sprite HEAD_SIDE("uni student head, facing right", {
"....HHHHHH......",
"...HHHHHHHH.....",
"..HHHHHHHHHH....",
"..HHHHHHHHhSS...",
"..HHHHHHhSSSS...",
"..HHHHHSSSSES...",
"..HHHsSSSSSSSS..",
"...HHsSSSSSSS...",
"....sSSSSSSMS...",
".....sSSSSSS....",
"......sSSS......",
"......sSS.......",
}, { {'H', {168, 92, 44}}, {'h', {206, 128, 64}}, {'S', {246, 204, 164}}, {'s', {214, 164, 128}}, {'E', {34, 26, 34}}, {'M', {176, 80, 84}} });
struct Season { RGB skyT, skyB, grass, grassD, grassL, water, waterD, waterL, leaf, leafD, leafL, top, topD; const char* name; };
static const Season SEASONS[4] = {
    { {138, 196, 240}, {214, 236, 250}, {122, 186, 92}, {96, 156, 72}, {156, 208, 112}, {96, 164, 214}, {70, 132, 188}, {176, 218, 242}, {244, 180, 200}, {222, 136, 166}, {254, 222, 232}, {120, 178, 132}, {90, 142, 104}, "SPRING" },
    { {86, 166, 238}, {176, 222, 250}, {84, 166, 70}, {62, 134, 56}, {122, 198, 88}, {62, 150, 212}, {44, 116, 180}, {160, 214, 246}, {72, 150, 66}, {50, 116, 52}, {112, 186, 84}, {244, 242, 236}, {204, 202, 196}, "SUMMER" },
    { {132, 176, 224}, {244, 212, 168}, {176, 164, 84}, {146, 130, 64}, {204, 190, 108}, {82, 132, 172}, {62, 104, 146}, {170, 196, 214}, {232, 122, 52}, {190, 82, 42}, {250, 184, 72}, {156, 100, 62}, {118, 74, 46}, "AUTUMN" },
    { {166, 186, 212}, {224, 232, 242}, {236, 240, 248}, {200, 210, 228}, {252, 252, 255}, {126, 156, 186}, {100, 128, 160}, {204, 222, 238}, {250, 252, 255}, {206, 216, 232}, {255, 255, 255}, {44, 54, 96}, {30, 38, 70}, "WINTER" },
};
static void outlineCopy(Frame& f, const Frame& tmp, uint8_t key, int w, int h, int dx, int dy, bool flip) {
    uint8_t ol = C(OL);
    for (int j = -1; j <= h; ++j) for (int i = -1; i <= w; ++i) {
        int si = flip ? w - 1 - i : i;
        uint8_t c = (si >= 0 && si < w && j >= 0 && j < h) ? tmp.get(si, j) : key;
        if (c != key) { f.set(dx + i, dy + j, c); continue; }
        bool edge = false;
        for (int k = 0; k < 4 && !edge; ++k) {
            int ni = si + (k == 0) - (k == 1), nj = j + (k == 2) - (k == 3);
            if (ni >= 0 && ni < w && nj >= 0 && nj < h && tmp.get(ni, nj) != key) edge = true;
        }
        if (edge) f.set(dx + i, dy + j, ol);
    }
}
static void recolor(Frame& tmp, int w, int h, uint8_t from, uint8_t to) { for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) if (tmp.get(i, j) == from) tmp.set(i, j, to); }
static void hairOn(Frame& tmp, int w, int h, RGB hair, RGB hairL) { recolor(tmp, w, h, C(168, 92, 44), C(hair)); recolor(tmp, w, h, C(206, 128, 64), C(hairL)); }
static void beanie(Frame& tmp, int ox, int oy) {
    uint8_t b = C(176, 52, 60), bD = C(136, 36, 46), bL = C(214, 92, 96);
    tmp.rect(ox + 2, oy - 1, 10, 5, b); tmp.rect(ox + 3, oy - 2, 8, 1, b); tmp.hline(ox + 2, ox + 11, oy + 3, bD); tmp.hline(ox + 3, ox + 10, oy - 1, bL);
    tmp.disc(ox + 6, oy - 3, 1, C(250, 248, 240));
}
static void sitter(Frame& f, int cx, int feet, int season, int typing, bool glow, bool smile, bool open) {
    static Frame tmp;
    const int W = 32, H = 34;
    const Season& s = SEASONS[season];
    uint8_t key = C(255, 0, 255), top = C(s.top), topD = C(s.topD), jean = C(70, 92, 134), jeanD = C(52, 70, 106), shoe = C(232, 232, 226), shoeD = C(170, 170, 170), skin = C(246, 204, 164), lap = C(176, 182, 192), lapD = C(118, 124, 136), lapL = C(220, 224, 232);
    tmp.rect(0, 0, W, H, key);
    int ox = 3, oy = 3, fy = H - 1;
    tmp.rect(ox + 4, oy + 20, 16, 5, jean); tmp.hline(ox + 4, ox + 19, oy + 24, jeanD);
    tmp.rect(ox + 17, oy + 22, 4, 9, jeanD); tmp.rect(ox + 18, oy + 22, 3, 8, jean);
    tmp.rect(ox + 17, fy - 2, 7, 3, shoe); tmp.hline(ox + 17, ox + 23, fy, shoeD);
    tmp.rect(ox + 3, oy + 11, 9, 11, top); tmp.vline(ox + 3, oy + 11, oy + 21, topD); tmp.hline(ox + 3, ox + 11, oy + 21, topD);
    if (season == 0) { tmp.rect(ox + 1, oy + 9, 4, 6, topD); tmp.vline(ox + 9, oy + 12, oy + 17, C(250, 250, 244)); }
    if (season == 2) { tmp.vline(ox + 10, oy + 11, oy + 21, topD); tmp.rect(ox + 9, oy + 11, 2, 3, C(230, 224, 210)); }
    blit(tmp, HEAD_SIDE, ox, oy);
    hairOn(tmp, W, H, { 168, 92, 44 }, { 206, 128, 64 });
    if (season == 3) { beanie(tmp, ox, oy); tmp.rect(ox + 4, oy + 10, 8, 3, C(200, 50, 60)); tmp.rect(ox + 4, oy + 12, 3, 5, C(170, 36, 48)); tmp.hline(ox + 4, ox + 11, oy + 10, C(230, 92, 92)); }
    if (smile) { tmp.set(ox + 11, oy + 8, C(176, 80, 84)); tmp.set(ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); }
    if (glow) for (int j = oy + 3; j < oy + 12; ++j) for (int i = ox + 11; i < ox + 15; ++i) if (tmp.get(i, j) == skin) tmp.set(i, j, C(214, 232, 250));
    if (open) {
        tmp.rect(ox + 10, oy + 18, 11, 2, lapD); tmp.hline(ox + 10, ox + 20, oy + 18, lapL);
        tmp.line(ox + 20, oy + 18, ox + 23, oy + 8, lap); tmp.line(ox + 21, oy + 18, ox + 24, oy + 8, lapD);
        if (glow) { tmp.line(ox + 19, oy + 17, ox + 22, oy + 8, C(170, 214, 252)); }
    } else {
        tmp.rect(ox + 10, oy + 17, 11, 3, lapD); tmp.hline(ox + 10, ox + 20, oy + 17, lapL);
    }
    int jx = typing > 0 ? (typing % 2 ? 1 : -1) : 0;
    int sx = ox + 7, sy = oy + 13, ax = ox + 13 + jx, ay = oy + 17;
    tmp.taper(sx, sy, ax, ay, 3, 3, season == 1 ? skin : topD);
    if (season == 1) tmp.taper(sx, sy, sx + 2, sy + 3, 4, 3, top);
    tmp.rect(ax - 1, ay - 1, 2, 2, skin);
    outlineCopy(f, tmp, key, W, H, cx - (ox + 8), feet - fy, false);
}
static void stander(Frame& f, int cx, int feet, int season, RGB hair, bool flip, int step, bool bag, bool smile) {
    static Frame tmp;
    const int W = 24, H = 46;
    const Season& s = SEASONS[season];
    uint8_t key = C(255, 0, 255), top = C(s.top), topD = C(s.topD), jean = C(70, 92, 134), jeanD = C(52, 70, 106), shoe = C(232, 232, 226), skin = C(246, 204, 164), pack = C(60, 66, 80), packD = C(40, 44, 56), packL = C(96, 104, 120);
    tmp.rect(0, 0, W, H, key);
    int ox = 4, oy = 2, hy = oy + 26, fy = H - 1;
    int st = step % 4, sw = st == 1 ? 3 : st == 3 ? -3 : 0;
    auto leg = [&](int fx, uint8_t c) { tmp.taper(ox + 7, hy, fx, fy - 2, 3, 3, c); tmp.rect(fx - 1, fy - 1, 4, 2, shoe); tmp.set(fx - 1, fy, C(170, 170, 170)); };
    leg(ox + 7 - sw, jeanD);
    leg(ox + 8 + sw, jean);
    if (bag) { tmp.rect(ox - 1, oy + 13, 5, 12, pack); tmp.vline(ox - 1, oy + 13, oy + 24, packD); tmp.hline(ox - 1, ox + 3, oy + 13, packL); }
    tmp.rect(ox + 3, oy + 12, 10, 15, top); tmp.vline(ox + 3, oy + 12, oy + 26, topD); tmp.hline(ox + 3, ox + 12, oy + 26, topD);
    if (bag) tmp.line(ox + 4, oy + 13, ox + 6, oy + 22, packD);
    blit(tmp, HEAD_SIDE, ox, oy);
    hairOn(tmp, W, H, hair, RGB{ std::min(255, hair.r + 30), std::min(255, hair.g + 24), std::min(255, hair.b + 20) });
    if (season == 3) { beanie(tmp, ox, oy); tmp.rect(ox + 4, oy + 10, 8, 3, C(200, 50, 60)); tmp.hline(ox + 4, ox + 11, oy + 10, C(230, 92, 92)); }
    if (smile) { tmp.set(ox + 11, oy + 8, C(176, 80, 84)); tmp.set(ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); }
    int sx = ox + 7, sy = oy + 14, ax = sx - sw, ay = sy + 9;
    tmp.taper(sx, sy, ax, ay, 4, 3, season == 1 ? skin : topD);
    if (season == 1) tmp.taper(sx, sy, sx - sw / 2, sy + 3, 4, 4, top);
    tmp.rect(ax - 1, ay, 2, 2, skin);
    f.ellipse(cx, feet + 1, 7, 2, C(OL));
    outlineCopy(f, tmp, key, W, H, flip ? cx - (W - 1 - (ox + 8)) : cx - (ox + 8), feet - fy, flip);
}
static void gradSprite(Frame& f, int cx, int feet, RGB hair, bool flip, int pose, bool cap, bool smile) {
    static Frame tmp;
    const int W = 24, H = 52;
    uint8_t key = C(255, 0, 255), gown = C(34, 42, 92), gownD = C(24, 30, 66), gownL = C(58, 70, 130), stole = C(232, 188, 72), stoleD = C(186, 140, 46), board = C(26, 26, 34), boardL = C(64, 64, 80), skin = C(246, 204, 164), shoe = C(30, 26, 30), paper = C(252, 250, 240), paperD = C(214, 208, 192), ribbon = C(200, 52, 60);
    tmp.rect(0, 0, W, H, key);
    int ox = 3, oy = 8, fy = H - 1;
    int sx = ox + 7, sy = oy + 14, ax = sx + 3, ay = sy + 9;
    if (pose == 1) { ax = sx + 5; ay = oy - 2; }
    auto arm = [&]() {
        tmp.taper(sx, sy, ax, ay, 4, 3, gownD); tmp.disc(ax, ay, 1, skin);
        int px = ax - 4, py = pose == 1 ? ay - 4 : ay - 1;
        tmp.rect(px, py, 9, 3, paper); tmp.hline(px, px + 8, py + 2, paperD); tmp.vline(px + 4, py, py + 2, ribbon);
    };
    if (pose == 1) arm();
    tmp.poly({ {ox + 4, oy + 12}, {ox + 11, oy + 12}, {ox + 14, fy - 2}, {ox + 1, fy - 2} }, gown);
    tmp.vline(ox + 5, oy + 18, fy - 3, gownD); tmp.vline(ox + 9, oy + 20, fy - 3, gownD); tmp.vline(ox + 12, oy + 22, fy - 3, gownL);
    tmp.hline(ox + 1, ox + 14, fy - 2, gownD);
    tmp.rect(ox + 3, fy - 1, 4, 1, shoe); tmp.rect(ox + 9, fy - 1, 4, 1, shoe);
    tmp.poly({ {ox + 9, oy + 12}, {ox + 11, oy + 12}, {ox + 12, oy + 26}, {ox + 10, oy + 26} }, stole); tmp.vline(ox + 10, oy + 14, oy + 25, stoleD);
    blit(tmp, HEAD_SIDE, ox, oy);
    hairOn(tmp, W, H, hair, RGB{ std::min(255, hair.r + 30), std::min(255, hair.g + 24), std::min(255, hair.b + 20) });
    if (smile) { tmp.set(ox + 11, oy + 8, C(176, 80, 84)); tmp.set(ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); tmp.set(ox + 11, oy + 5, C(34, 26, 34)); }
    if (cap) {
        tmp.rect(ox + 2, oy - 1, 10, 3, board); tmp.hline(ox + 2, ox + 11, oy - 1, boardL);
        tmp.rect(ox - 1, oy - 3, 16, 2, board); tmp.hline(ox - 1, ox + 14, oy - 3, boardL);
        tmp.set(ox + 7, oy - 4, stole); tmp.vline(ox, oy - 2, oy + 3, stole); tmp.rect(ox - 1, oy + 3, 2, 2, stole);
    }
    if (pose != 1) arm();
    outlineCopy(f, tmp, key, W, H, flip ? cx - (W - 1 - (ox + 8)) : cx - (ox + 8), feet - fy, flip);
}
static void flyingCap(Frame& f, int x, int y, int spin) {
    uint8_t ol = C(OL), board = C(26, 26, 34), boardL = C(70, 70, 90), gold = C(232, 188, 72);
    int w = spin % 4 == 0 ? 7 : spin % 4 == 2 ? 3 : 5;
    f.rect(x - w - 1, y - 2, w * 2 + 3, 4, ol); f.rect(x - w, y - 1, w * 2 + 1, 2, board); f.hline(x - w, x + w, y - 1, boardL);
    f.rect(x - 3, y + 1, 7, 3, ol); f.rect(x - 2, y + 1, 5, 2, board);
    f.vline(x + w - 1, y, y + 3 + spin % 2, gold);
}
static void shade(Frame& f, int x0, int y0, int x1, int y1, int level) {
    if (level <= 0) return;
    static const RGB lo[3] = { {70, 52, 84}, {34, 34, 70}, {12, 16, 40} }, hi[3] = { {246, 206, 196}, {184, 180, 222}, {118, 138, 196} };
    static const RGB keep[12] = { {250, 214, 120}, {200, 150, 70}, {255, 236, 160}, {170, 214, 252}, {214, 232, 250}, {244, 240, 214}, {230, 230, 250}, {120, 120, 160}, {250, 220, 120}, {12, 16, 38}, {22, 28, 60}, {40, 46, 88} };
    int l = std::min(3, level) - 1;
    int map[256];
    for (int& m : map) m = -1;
    for (int y = std::max(0, y0); y < std::min(270, y1); ++y) for (int x = std::max(0, x0); x < std::min(480, x1); ++x) {
        uint8_t c = f.get(x, y);
        if (map[c] < 0) {
            RGB v = palette()[c];
            bool lit = false;
            for (auto& k : keep) if (k.r == v.r && k.g == v.g && k.b == v.b) lit = true;
            if (lit) map[c] = c;
            else {
                float lum = (v.r * 0.3f + v.g * 0.59f + v.b * 0.11f) / 255.0f;
                int q = std::min(7, (int)(lum * 8));
                map[c] = C(mix(lo[l], hi[l], q / 7.0f));
            }
        }
        f.set(x, y, (uint8_t)map[c]);
    }
}
void UniRound::reset() { queue_.clear(); curT_ = -1; last_ = 0; gradT_ = -1; lines_ = 0; shown_ = 0; season_ = 0; cups_ = 0; nightT_ = -1000; nightLine_ = 0; lineT_ = 0; }
void UniRound::onLinesCleared(const std::vector<int>&, int totalLines, int) {
    while (lines_ < totalLines && lines_ < 12) {
        ++lines_;
        queue_.push_back({ TYPE, lines_ });
        if (lines_ == 12) queue_.push_back({ GRAD, lines_ });
    }
}
void UniRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t;
    if (cur_.k == GRAD) gradT_ = t;
    if (cur_.k != TYPE) return;
    shown_ = cur_.line; lineT_ = t;
    if (shown_ % 3 == 0 && shown_ < 12) { nightT_ = t; nightLine_ = shown_; ++cups_; }
}
void UniRound::update(const PlayStateReader&, int t) {
    last_ = t;
    if (gradT_ >= 0) return;
    if (t - nightT_ == DAWN) season_ = std::min(3, nightLine_ / 3);
    if (curT_ >= 0) {
        if (cur_.k == GRAD || t - curT_ < TYPE_LEN) return;
        curT_ = -1;
    }
    if (queue_.empty()) return;
    if (queue_.front().k == TYPE && queue_.front().line % 3 == 0 && t - nightT_ < DAWN + 8) return;
    start(t);
}
bool UniRound::playing(int t) const {
    (void)t;
    return gradT_ < 0;
}
int UniRound::night(int t) const {
    int a = t - nightT_;
    if (a < 0 || a >= DAWN) return 0;
    if (a < 3) return 1;
    if (a < 6) return 2;
    if (a < DAWN - 2) return 3;
    if (a < DAWN) return 2;
    return 0;
}
void UniRound::drawSky(Frame& f, int t, int n) {
    const Season& s = SEASONS[season_];
    if (n >= 3) {
        f.gradient(0, 0, 480, 104, { {12, 16, 38}, {22, 28, 60}, {40, 46, 88} });
        for (int k = 0; k < 40; ++k) { int x = (k * 97) % 480, y = (k * 37) % 90; f.set(x, y, (k + t / 3) % 7 ? C(230, 230, 250) : C(120, 120, 160)); }
        f.disc(250, 26, 8, C(244, 240, 214)); f.disc(254, 23, 7, C(22, 28, 60));
        return;
    }
    f.gradient(0, 0, 480, 104, { s.skyT, mix(s.skyT, s.skyB, 0.5f), s.skyB });
    uint8_t cl = C(250, 252, 255), clD = C(214, 226, 240);
    for (int k = 0; k < 3; ++k) {
        int cx = ((k * 170 + t / (3 + k)) % 620) - 70, cy = 14 + k * 12;
        f.ellipse(cx, cy + 2, 22, 5, clD); f.ellipse(cx - 8, cy, 10, 5, cl); f.ellipse(cx + 6, cy - 2, 12, 6, cl); f.ellipse(cx + 16, cy + 1, 8, 4, cl);
    }
    if (season_ == 1) { f.disc(440, 22, 10, C(255, 236, 150)); f.disc(440, 22, 7, C(255, 250, 214)); }
}
void UniRound::drawCampus(Frame& f, int t, int n) {
    const Season& s = SEASONS[season_];
    uint8_t ol = C(OL), brick = C(170, 82, 64), brickD = C(136, 60, 50), brickL = C(198, 112, 88), roof = C(70, 78, 92), roofL = C(100, 110, 126), stone = C(232, 224, 206), stoneD = C(196, 186, 166), win = n >= 3 ? C(250, 214, 120) : C(150, 190, 224), winD = n >= 3 ? C(200, 150, 70) : C(110, 150, 196);
    int base = 102;
    f.rect(0, base - 6, 480, 8, C(s.grassD));
    auto block = [&](int x0, int x1, int top) {
        f.rect(x0 - 1, top - 1, x1 - x0 + 2, base - top + 1, ol); f.rect(x0, top, x1 - x0, base - top, brick);
        for (int y = top + 3; y < base; y += 4) f.hline(x0, x1 - 1, y, brickD);
        f.hline(x0, x1 - 1, top, brickL);
        f.poly({ {x0 - 3, top}, {x1 + 2, top}, {x1 - 2, top - 5}, {x0 + 1, top - 5} }, roof); f.hline(x0, x1 - 1, top - 5, roofL);
        for (int wx = x0 + 4; wx + 4 < x1; wx += 9) for (int wy = top + 6; wy + 6 < base - 2; wy += 12) {
            bool lit = n < 3 || ((wx * 7 + wy * 3) % 5 < 2);
            f.rect(wx, wy, 5, 7, ol); f.rect(wx + 1, wy + 1, 3, 5, lit ? win : C(30, 34, 56)); f.set(wx + 1, wy + 1, lit ? winD : C(30, 34, 56));
        }
    };
    block(4, 90, 70);
    block(200, 290, 64);
    block(410, 480, 60);
    f.rect(296, 39, 108, 64, ol); f.rect(297, 40, 106, 62, stone); f.hline(297, 402, 40, C(250, 246, 234));
    for (int y = 46; y < 102; y += 6) f.hline(297, 402, y, stoneD);
    f.poly({ {292, 40}, {408, 40}, {350, 26} }, ol); f.poly({ {296, 39}, {404, 39}, {350, 28} }, stone); f.line(296, 39, 350, 28, C(250, 246, 234));
    for (int k = 0; k < 6; ++k) { int cx = 306 + k * 18; f.rect(cx - 3, 44, 7, 52, ol); f.rect(cx - 2, 44, 5, 52, C(248, 244, 232)); f.vline(cx + 2, 44, 95, stoneD); }
    f.rect(296, 96, 108, 6, stoneD); f.hline(296, 403, 96, ol);
    f.rect(338, 2, 24, 28, ol); f.rect(339, 3, 22, 26, stone); f.poly({ {336, 3}, {364, 3}, {350, -10} }, roof);
    f.disc(350, 15, 7, ol); f.disc(350, 15, 6, C(250, 248, 236)); f.vline(350, 11, 15, ol); f.hline(350, 353, 15, ol);
    f.rect(0, base, 480, 46, C(s.water));
    for (int y = base; y < base + 46; ++y) {
        int sy = base - 1 - (y - base) * 2;
        if (sy < 0 || (y - base) % 3 == 2) continue;
        for (int x = 0; x < 480; ++x) {
            uint8_t c = f.get(x + rnd(std::sin((y + t) * 0.6f) * 1.5f), sy);
            if (c == C(s.skyB) || c == C(s.skyT)) continue;
            if ((x + y) % 2 == 0) f.set(x, y, c);
        }
    }
    for (int y = base; y < base + 46; y += 2) for (int x = (y / 2) % 2; x < 480; x += 2) f.set(x, y, C(s.waterD));
    for (int k = 0; k < 18; ++k) { int x = 4 + (k * 61 + t * (k % 2 ? 1 : -1) + 960) % 470, y = base + 4 + (k * 13) % 40; if (x < 474) f.hline(x, x + 4 + k % 3, y, C(s.waterL)); }
    if (season_ == 3) for (int k = 0; k < 6; ++k) { int x = 200 + k * 46, y = base + 8 + (k * 11) % 30; f.hline(x, x + 22, y, C(232, 240, 250)); f.hline(x + 4, x + 16, y + 1, C(214, 226, 240)); }
    if (season_ <= 1) for (int k = 0; k < 2; ++k) {
        int dx = 260 + k * 18 + rnd(std::sin((t + k * 20) * 0.05f) * 30), dy = base + 22 + k * 6;
        f.ellipse(dx, dy, 5, 2, ol); f.ellipse(dx, dy, 4, 1, C(250, 250, 244)); f.disc(dx + 4, dy - 3, 2, ol); f.disc(dx + 4, dy - 3, 1, C(70, 120, 70)); f.set(dx + 6, dy - 3, C(240, 170, 60));
    }
    f.rect(0, base + 46, 480, 124, C(s.grass));
    f.hline(0, 479, base + 46, C(s.grassD)); f.hline(0, 479, base + 47, C(s.grassL));
    for (int k = 0; k < 70; ++k) {
        int x = 4 + (k * 53) % 474, y = base + 52 + (k * 31) % 114;
        if (season_ == 3) { f.hline(x, x + 3, y, C(s.grassD)); continue; }
        f.vline(x, y, y + 2, C(s.grassD)); f.set(x + 1, y, C(s.grassD)); f.set(x - 1, y + 1, C(s.grassL));
    }
    uint8_t lane = season_ == 3 ? C(220, 228, 240) : C(204, 190, 160), laneD = season_ == 3 ? C(196, 206, 222) : C(176, 160, 130);
    f.rect(0, 157, 480, 12, lane); f.hline(0, 479, 157, laneD); f.hline(0, 479, 168, laneD);
    for (int x = 0; x < 480; x += 10) f.vline(x + ((x / 10) % 2) * 5, 158, 167, laneD);
    uint8_t path = season_ == 3 ? C(212, 220, 234) : C(214, 200, 170), pathD = season_ == 3 ? C(190, 200, 218) : C(184, 168, 138);
    f.rect(0, 250, 480, 14, path); f.hline(0, 479, 250, pathD); f.hline(0, 479, 263, pathD);
    for (int x = 0; x < 480; x += 14) f.vline(x + ((x / 14) % 2) * 7, 251, 262, pathD);
    if (season_ == 0) for (int k = 0; k < 14; ++k) { int x = 200 + (k * 71) % 270, y = 160 + (k * 29) % 80; f.set(x, y, C(250, 240, 120)); f.set(x + 1, y, C(250, 250, 244)); }
    if (season_ == 2) for (int k = 0; k < 20; ++k) { int x = 196 + (k * 47) % 280, y = 156 + (k * 37) % 90; f.rect(x, y, 2, 1, k % 2 ? C(s.leaf) : C(s.leafL)); }
}
void UniRound::drawTree(Frame& f, int t) {
    const Season& s = SEASONS[season_];
    uint8_t ol = C(OL), bark = C(96, 70, 56), barkD = C(70, 50, 42), barkL = C(124, 94, 74);
    int x = 446, feet = 236;
    f.taper(x, feet, x - 4, feet - 80, 14, 9, ol); f.taper(x, feet, x - 4, feet - 80, 12, 7, bark); f.vline(x - 6, feet - 70, feet - 4, barkD); f.vline(x + 2, feet - 60, feet - 6, barkL);
    for (int k = 0; k < 6; ++k) { int y = feet - 10 - k * 12; f.hline(x - 4, x - 1, y, barkD); }
    f.taper(x - 3, feet - 60, x - 40, feet - 96, 5, 3, ol); f.taper(x - 3, feet - 60, x - 40, feet - 96, 4, 2, bark);
    f.taper(x - 3, feet - 70, x + 30, feet - 104, 5, 3, ol); f.taper(x - 3, feet - 70, x + 30, feet - 104, 4, 2, bark);
    f.taper(x - 4, feet - 78, x - 14, feet - 120, 4, 2, ol); f.taper(x - 4, feet - 78, x - 14, feet - 120, 3, 1, bark);
    f.ellipse(x - 6, feet + 1, 22, 3, C(s.grassD));
    static const int bx[12] = { -44, -30, -14, 2, 18, 30, -38, -20, -2, 16, -26, 4 }, by[12] = { -98, -112, -122, -126, -114, -100, -84, -94, -104, -92, -130, -140 }, br[12] = { 13, 15, 16, 15, 14, 12, 11, 14, 15, 12, 11, 10 };
    if (season_ == 3) {
        uint8_t snow = C(252, 252, 255), snowD = C(214, 222, 236);
        f.taper(x - 40, feet - 96, x - 52, feet - 104, 3, 1, ol); f.taper(x + 30, feet - 104, x + 40, feet - 116, 3, 1, ol);
        f.hline(x - 44, x - 30, feet - 99, snow); f.hline(x + 14, x + 30, feet - 107, snow); f.hline(x - 18, x - 10, feet - 118, snow); f.hline(x - 8, x + 4, feet - 82, snow);
        f.hline(x - 43, x - 32, feet - 98, snowD);
        return;
    }
    int sway = rnd(std::sin(t * 0.12f) * 1.2f);
    for (int k = 0; k < 12; ++k) f.disc(x + bx[k] + sway, feet + by[k], br[k] + 1, ol);
    for (int k = 0; k < 12; ++k) f.disc(x + bx[k] + sway, feet + by[k], br[k], k % 3 == 0 ? C(s.leafD) : C(s.leaf));
    for (int k = 0; k < 12; ++k) f.disc(x + bx[k] - 3 + sway, feet + by[k] - 3, br[k] - 6, C(s.leafL));
    for (int k = 0; k < 12; ++k) for (int j = 0; j < 3; ++j) f.set(x + bx[k] + sway + (j * 5 - 5), feet + by[k] + br[k] - 3 - j % 2, C(s.leafD));
}
void UniRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    if (ceremony(t)) { drawHall(f, t); Stage2UI::pills(f, t); drawCommonUI(f); return; }
    int n = night(t);
    drawSky(f, t, n);
    drawCampus(f, t, n);
    drawTree(f, t);
    shade(f, 0, 0, 480, 270, n);
    Stage2UI::pills(f, t);
    drawCommonUI(f);
}
void UniRound::drawBoardFrame(Frame& f, int t) {
    RoundScene::drawBoardFrame(f, t);
    if (season_ != 3 || ceremony(t)) return;
    int bx = boardX(), by = boardY();
    uint8_t snow = C(252, 252, 255), snowD = C(214, 222, 236);
    f.rect(bx - 8, by - 10, 96, 4, snow); f.hline(bx - 8, bx + 87, by - 7, snowD);
    for (int k = 0; k < 8; ++k) { int x = bx - 6 + k * 12; f.rect(x, by - 7, 4, 2 + k % 2, snow); }
}
void UniRound::drawStudent(Frame& f, int t, int n) {
    uint8_t ol = C(OL);
    for (int k = 0; k < cups_; ++k) {
        int cx = SIT_X - 38 + (k % 2) * 7, cy = SIT_Y - (k / 2) * 8;
        f.rect(cx - 1, cy - 8, 7, 9, ol); f.rect(cx, cy - 7, 5, 7, C(250, 248, 240)); f.rect(cx, cy - 4, 5, 2, C(150, 96, 62)); f.hline(cx - 1, cx + 5, cy - 8, ol);
    }
    f.ellipse(SIT_X - 20, SIT_Y, 7, 2, ol);
    f.rect(SIT_X - 25, SIT_Y - 13, 10, 13, ol); f.rect(SIT_X - 24, SIT_Y - 12, 8, 12, C(60, 66, 80)); f.hline(SIT_X - 24, SIT_X - 17, SIT_Y - 12, C(96, 104, 120)); f.rect(SIT_X - 23, SIT_Y - 7, 6, 4, C(40, 44, 56));
    drawBench(f);
    int typing = 0;
    if (curT_ >= 0 && cur_.k == TYPE && t - curT_ < TYPE_LEN) typing = t - curT_ + 1;
    if (n >= 2) typing = t;
    sitter(f, SIT_X, SIT_Y, season_, typing, n >= 2, typing == 0 && n == 0, true);
    if (n >= 2 && (t / 2) % 2) Font::draw(f, "z", SIT_X + 12, SIT_Y - 44 - (t % 6), C(220, 230, 250));
}
void UniRound::drawWalkers(Frame& f, int t) {
    static const RGB hair[5] = { {44, 34, 44}, {96, 60, 40}, {30, 26, 30}, {140, 96, 56}, {20, 20, 24} };
    for (int k = 0; k < 3; ++k) {
        int span = 340, a = (t * 2 + k * 120) % (span + 200);
        if (a > span) continue;
        bool left = k % 2;
        int x = left ? 480 - a : 190 + a - 40;
        if (x < 196 || x > 476) continue;
        stander(f, x, 167, season_, hair[(k + season_) % 5], left, a / 3, true, false);
    }
}
void UniRound::drawWeather(Frame& f, int t) {
    const Season& s = SEASONS[season_];
    for (int k = 0; k < 40; ++k) {
        int x = (k * 67 + t * (1 + k % 2) + rnd(std::sin((t + k * 9) * 0.15f) * 6)) % 480, y = (k * 31 + t * (season_ == 3 ? 1 + k % 2 : 2 + k % 3)) % 270;
        if ((x > 88 && x < 192 && y > 48 && y < 232) || (season_ == 1 && k % 4)) continue;
        if (season_ == 0) { f.set(x, y, C(s.leafL)); if (k % 2) f.set(x + 1, y, C(s.leaf)); }
        else if (season_ == 1) { if ((t + k) % 8 < 2) { f.set(x, y, C(255, 252, 220)); f.set(x - 1, y, C(255, 240, 170)); f.set(x + 1, y, C(255, 240, 170)); } }
        else if (season_ == 2) { if (k % 2) f.rect(x, y, 2, 1, C(s.leaf)); else f.rect(x, y, 1, 2, C(s.leafD)); }
        else { f.set(x, y, C(255, 255, 255)); if (k % 3 == 0) { f.set(x + 1, y, C(232, 238, 248)); f.set(x, y + 1, C(232, 238, 248)); } }
    }
}
void UniRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    if (ceremony(t)) { drawCeremony(f, t); return; }
    int n = night(t);
    if (gradT_ >= 0) {
        int a = t - gradT_;
        drawBench(f);
        if (a < STAND) sitter(f, SIT_X, SIT_Y, season_, 0, false, true, a < 4);
        else stander(f, SIT_X + (a - STAND) * 2, SIT_Y, season_, { 168, 92, 44 }, false, (a - STAND) / 2, true, true);
        drawWeather(f, t);
        return;
    }
    if (n == 0) drawWalkers(f, t);
    if (n > 0) shade(f, 190, 120, 480, 172, n);
    drawStudent(f, t, n);
    if (n >= 3) {
        uint8_t glow = C(170, 214, 252);
        for (int k = 0; k < 6; ++k) f.set(SIT_X + 12 + k, SIT_Y - 26 + k / 2, glow);
    }
    if (n == 0) drawWeather(f, t);
}
void UniRound::drawNext(Frame& f, int piece, int t) {
    if (gradT_ >= 0) return;
    uint8_t ol = C(OL), cork = C(196, 150, 96), corkD = C(168, 122, 74), wood = C(120, 80, 52), woodL = C(156, 108, 70), paper = C(250, 248, 236), pin = C(220, 60, 60);
    int x = 206, y = 132;
    f.rect(x + 8, y + 50, 4, 66, ol); f.rect(x + 52, y + 50, 4, 66, ol); f.rect(x + 9, y + 50, 2, 66, wood); f.rect(x + 53, y + 50, 2, 66, wood);
    f.rect(x - 1, y - 1, 66, 54, ol); f.rect(x, y, 64, 52, wood); f.hline(x, x + 63, y, woodL); f.rect(x + 3, y + 3, 58, 46, cork);
    for (int k = 0; k < 30; ++k) f.set(x + 4 + (k * 17) % 56, y + 4 + (k * 11) % 44, corkD);
    f.rect(x + 7, y + 6, 18, 12, paper); f.hline(x + 9, x + 22, y + 9, C(150, 160, 190)); f.hline(x + 9, x + 19, y + 12, C(150, 160, 190)); f.disc(x + 16, y + 6, 1, C(70, 130, 230));
    bool blown = nextBlown(t);
    if (blown) {
        int a = (t % 120) - 40, px = x + 28 + a * 5, py = y + 6 - a * 2 + rnd(std::sin(a * 0.7f) * 4);
        if (px < 470) { f.rect(px, py, 16, 12, ol); f.rect(px + 1, py + 1, 14, 10, paper); f.line(px + 2, py + 3, px + 12, py + 4, C(150, 160, 190)); }
        f.disc(x + 43, y + 7, 1, pin);
        Font::center(f, "WINDY", x + 43, y + 22, C(250, 236, 200), ol);
        for (int k = 0; k < 6; ++k) { int lx = x + 20 + (k * 23 + a * 6) % 50, ly = y + 10 + (k * 13) % 30; f.rect(lx, ly, 2, 1, C(232, 122, 52)); }
        if (season_ == 3) { f.rect(x - 2, y - 4, 68, 4, C(252, 252, 255)); f.hline(x - 2, x + 65, y - 1, C(214, 222, 236)); }
        shade(f, x - 2, y - 5, x + 67, y + 117, night(t));
        return;
    }
    f.rect(x + 28, y + 6, 30, 40, ol); f.rect(x + 29, y + 7, 28, 38, paper); f.disc(x + 43, y + 7, 1, pin);
    Font::center(f, "NEXT", x + 43, y + 10, C(70, 60, 70));
    if (season_ == 3) { f.rect(x - 2, y - 4, 68, 4, C(252, 252, 255)); f.hline(x - 2, x + 65, y - 1, C(214, 222, 236)); }
    std::vector<std::pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    for (auto& c : cs) ToyBlocks::cell6(f, x + 43 - bw / 2 + (c.first - mx) * 6, y + 31 - bh / 2 + (c.second - my) * 6, piece);
    shade(f, x - 2, y - 5, x + 67, y + 117, night(t));
}
void UniRound::drawHud(Frame& f, int, int t) {
    if (gradT_ >= 0 && ceremony(t)) return;
    uint8_t ol = C(OL), pole = C(50, 56, 66), poleL = C(90, 98, 112), flag = C(28, 70, 60), flagD = C(20, 50, 44), gold = C(240, 204, 96), dim = C(60, 100, 90), lamp = night(t) >= 2 ? C(255, 236, 160) : C(230, 230, 220);
    int x = 274, y = 160;
    f.rect(x - 2, y - 30, 5, 106, ol); f.rect(x - 1, y - 30, 3, 106, pole); f.vline(x - 1, y - 30, y + 75, poleL);
    f.rect(x - 6, y - 40, 13, 10, ol); f.rect(x - 5, y - 39, 11, 8, lamp); f.poly({ {x - 8, y - 40}, {x + 8, y - 40}, {x, y - 46} }, ol);
    if (night(t) >= 2) for (int r = 6; r < 14; r += 3) for (int a = 0; a < 16; ++a) f.set(x + rnd(std::cos(a * 0.39f) * r), y - 35 + rnd(std::sin(a * 0.39f) * r), C(255, 236, 160));
    int bx = x + 3, by = y - 26;
    f.rect(bx, by - 1, 30, 62, ol); f.rect(bx + 1, by, 28, 58, flag); f.vline(bx + 28, by, by + 57, flagD);
    f.poly({ {bx + 1, by + 58}, {bx + 14, by + 58}, {bx + 1, by + 64} }, ol); f.poly({ {bx + 29, by + 58}, {bx + 16, by + 58}, {bx + 29, by + 64} }, ol);
    f.poly({ {bx + 2, by + 58}, {bx + 13, by + 58}, {bx + 2, by + 62} }, flag); f.poly({ {bx + 28, by + 58}, {bx + 17, by + 58}, {bx + 28, by + 62} }, flag);
    Font::center(f, "YEAR", bx + 15, by + 3, gold);
    Font::center(f, std::to_string(season_ + 1), bx + 15, by + 11, C(250, 250, 240));
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 3; ++c) {
        int i = r * 3 + c, cx = bx + 5 + c * 7, cy = by + 22 + r * 8;
        f.rect(cx, cy, 5, 5, i < shown_ ? gold : dim);
    }
}
void UniRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    if (gradT_ >= 0) flyToBook(f, t - gradT_ - TOSS - 4, 262, 192, Stage2UI::capItem, "GRAD CAP!", t);
    uint8_t ol = C(OL);
    if (gradT_ < 0 && night(t) == 0) {
        if (season_ == 0) petalGust(f, t);
        else if (season_ == 1) sunGlare(f, t);
        else if (season_ == 3) frost(f, t);
    }
    if (curT_ >= 0 && cur_.k == TYPE && gradT_ < 0) {
        int a = t - curT_;
        if (a < TYPE_LEN) {
            int bx = SIT_X + 22, by = SIT_Y - 50 - std::min(a, 4);
            f.rect(bx - 1, by - 1, 22, 11, ol); f.rect(bx, by, 20, 9, C(250, 250, 244)); f.poly({ {bx + 2, by + 9}, {bx + 7, by + 9}, {bx + 1, by + 13} }, ol);
            Font::center(f, "OK", bx + 10, by + 2, C(60, 150, 80));
        }
    }
    if (gradT_ < 0 && t - nightT_ >= 0 && t - nightT_ < DAWN + 6) {
        int a = t - nightT_;
        if (night(t) >= 3) nightLamp(f, state());
        if (a >= 4 && a < 16 && (a / 3) % 3 != 2) Font::center(f, "ALL NIGHTER", 335, 118, C(250, 220, 120), ol);
        if (a >= DAWN - 2 && a < DAWN + 6) {
            Font::center(f, SEASONS[std::min(3, nightLine_ / 3)].name, 335, 118, C(250, 250, 240), ol);
            float k = a < DAWN ? (a - DAWN + 3) / 3.0f : 1 - (a - DAWN) / 7.0f;
            k = std::max(0.0f, std::min(1.0f, k)) * 0.8f;
            for (auto& c : palette()) c = mix(c, RGB{ 255, 250, 236 }, k);
        }
    }
    if (gradT_ >= 0) {
        int a = t - gradT_;
        if (a >= WHITE_OUT && a < WHITE_IN + 10) {
            float k = a < WHITE_IN ? (a - WHITE_OUT + 1) / (float)(WHITE_IN - WHITE_OUT) : 1 - (a - WHITE_IN) / 10.0f;
            k = std::max(0.0f, std::min(1.0f, k));
            for (auto& c : palette()) c = mix(c, RGB{ 255, 255, 255 }, k);
        }
        if (a >= END - 12) { float k = std::min(1.0f, (a - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
}
void UniRound::drawBench(Frame& f) {
    uint8_t ol = C(OL), wood = C(150, 100, 62), woodD = C(112, 72, 46), woodL = C(186, 132, 84), iron = C(50, 54, 62);
    int x0 = SIT_X - 14, x1 = SIT_X + 14, seat = SIT_Y - 10;
    f.ellipse(SIT_X, SIT_Y + 1, 18, 3, ol);
    f.rect(x0 + 1, seat + 2, 3, 9, ol); f.rect(x1 - 4, seat + 2, 3, 9, ol); f.vline(x0 + 2, seat + 2, SIT_Y, iron); f.vline(x1 - 3, seat + 2, SIT_Y, iron);
    f.rect(x0 - 6, seat - 20, 4, 22, ol); f.vline(x0 - 5, seat - 19, seat + 1, woodD); f.vline(x0 - 4, seat - 19, seat + 1, wood);
    for (int k = 0; k < 3; ++k) { int y = seat - 18 + k * 6; f.rect(x0 - 7, y, 6, 4, ol); f.rect(x0 - 6, y + 1, 4, 2, k % 2 ? woodD : wood); }
    f.rect(x0 - 6, seat - 1, x1 - x0 + 7, 5, ol); f.rect(x0 - 5, seat, x1 - x0 + 5, 3, wood); f.hline(x0 - 5, x1 - 1, seat, woodL); f.hline(x0 - 5, x1 - 1, seat + 2, woodD);
    if (season_ == 3) { f.hline(x0 - 5, x0 + 4, seat - 1, C(252, 252, 255)); f.hline(x1 - 8, x1 - 1, seat - 1, C(252, 252, 255)); f.rect(x0 - 7, seat - 22, 6, 2, C(252, 252, 255)); }
}
void UniRound::nightLamp(Frame& f, const PlayStateReader& g) {
    int bx = boardX(), by = boardY(), cx = bx + 40, cy = by + 80, n = 0, sx = 0, sy = 0;
    for (auto& c : g.pieceCells()) { sx += c.first * 8 + 4; sy += c.second * 8 + 4; ++n; }
    if (n) { cx = bx + sx / n; cy = by + sy / n; }
    int map[256];
    for (int& m : map) m = -1;
    const int r = 30;
    for (int y = by; y < by + 160; ++y) for (int x = bx; x < bx + 80; ++x) {
        int dx = x - cx, dy = y - cy, d2 = dx * dx + dy * dy;
        if (d2 < r * r) continue;
        if (d2 < (r + 6) * (r + 6) && (x + y) % 2) continue;
        uint8_t c = f.get(x, y);
        if (map[c] < 0) {
            RGB v = palette()[c];
            float lum = (v.r * 0.3f + v.g * 0.59f + v.b * 0.11f) / 255.0f;
            map[c] = C(mix(RGB{ 6, 8, 20 }, RGB{ 46, 52, 90 }, std::min(7, (int)(lum * 8)) / 7.0f));
        }
        f.set(x, y, (uint8_t)map[c]);
    }
}
void UniRound::petalGust(Frame& f, int t) {
    int a = t % 96;
    if (a >= 36) return;
    const Season& s = SEASONS[0];
    uint8_t p0 = C(s.leaf), p1 = C(s.leafL), p2 = C(s.leafD);
    int bx = boardX(), by = boardY();
    float k = a < 6 ? a / 6.0f : a > 28 ? (36 - a) / 8.0f : 1.0f;
    int n = (int)(220 * k);
    for (int i = 0; i < n; ++i) {
        int x = bx - 10 + (i * 37 + a * (5 + i % 3)) % 100, y = by + (i * 53 + rnd(std::sin((a + i) * 0.4f) * 5)) % 160;
        if (x < bx || x > bx + 78) continue;
        f.set(x, y, i % 3 ? p0 : p1); f.set(x + 1, y, p0); f.set(x, y + 1, p2);
    }
    if (a < 10) Font::center(f, "GUST", bx + 40, by - 18, C(250, 236, 240), C(OL));
}
void UniRound::sunGlare(Frame& f, int t) {
    int a = t % 110;
    if (a >= 50) return;
    int bx = boardX(), by = boardY(), cx = bx + 10 + a * 60 / 50, cy = by + 20 + a * 120 / 50, r = 24;
    int map[256];
    for (int& m : map) m = -1;
    for (int y = cy - r - 4; y <= cy + r + 4; ++y) for (int x = cx - r - 4; x <= cx + r + 4; ++x) {
        if (x < bx || x >= bx + 80 || y < by || y >= by + 160) continue;
        int dx = x - cx, dy = y - cy, d2 = dx * dx + dy * dy;
        if (d2 > (r + 4) * (r + 4)) continue;
        if (d2 > r * r && (x + y) % 2) continue;
        uint8_t c = f.get(x, y);
        if (map[c] < 0) { RGB v = palette()[c]; float lum = (v.r * 0.3f + v.g * 0.59f + v.b * 0.11f) / 255.0f; map[c] = C(mix(RGB{ 236, 230, 200 }, RGB{ 255, 254, 244 }, std::min(3, (int)(lum * 4)) / 3.0f)); }
        f.set(x, y, (uint8_t)map[c]);
    }
    if (a < 10) Font::center(f, "GLARE", bx + 40, by - 18, C(255, 240, 170), C(OL));
}
void UniRound::frost(Frame& f, int t) {
    int bx = boardX(), by = boardY(), w = std::min(26, std::max(0, (t - lineT_ - 20) / 5));
    if (w <= 0) return;
    uint8_t f0 = C(226, 238, 250), f1 = C(196, 216, 240), f2 = C(250, 252, 255);
    for (int y = by; y < by + 160; ++y) for (int x = bx; x < bx + 80; ++x) {
        int e = std::min({ x - bx, bx + 79 - x, (y - by) * 2 });
        if (e >= w) continue;
        unsigned hsh = ((unsigned)x * 73856093u) ^ ((unsigned)y * 19349663u);
        int dens = 100 - e * 100 / w;
        if ((int)(hsh % 100) < dens) f.set(x, y, hsh % 7 == 0 ? f2 : hsh % 3 ? f0 : f1);
    }
    if (w == 26 && (t / 5) % 2) Font::center(f, "FROST", bx + 40, by - 18, C(220, 236, 255), C(OL));
}
bool UniRound::nextBlown(int t) const { return season_ == 2 && night(t) == 0 && gradT_ < 0 && t % 120 >= 40 && t % 120 < 84; }
void UniRound::drawHall(Frame& f, int t) {
    uint8_t ol = C(OL), brick = C(170, 82, 64), brickD = C(136, 60, 50), brickL = C(198, 112, 88), stone = C(236, 228, 210), stoneD = C(200, 190, 170), stoneL = C(252, 248, 236), roof = C(70, 78, 92), roofL = C(100, 110, 126), glass = C(132, 184, 224), glassD = C(100, 150, 200), red = C(176, 44, 52), redD = C(136, 30, 40), redL = C(210, 76, 80), gold = C(232, 188, 72);
    f.gradient(0, 0, 480, 40, { {138, 196, 240}, {200, 228, 248} });
    f.rect(0, 26, 480, 86, ol); f.rect(0, 27, 480, 85, brick);
    for (int y = 30; y < 112; y += 4) { f.hline(0, 479, y, brickD); for (int x = (y / 4 % 2) * 6; x < 480; x += 12) f.set(x, y + 2, brickD); }
    f.hline(0, 479, 27, brickL);
    f.rect(0, 20, 480, 7, ol); f.rect(0, 21, 480, 5, roof); f.hline(0, 479, 21, roofL);
    for (int c = 0; c < 15; ++c) {
        int wx = 8 + c * 32;
        if (wx > 180 && wx < 316) continue;
        for (int r = 0; r < 2; ++r) {
            int wy = 36 + r * 36;
            f.rect(wx - 1, wy - 3, 18, 27, ol); f.rect(wx, wy, 16, 23, glass); f.ellipse(wx + 8, wy, 8, 4, ol); f.ellipse(wx + 8, wy + 1, 7, 3, glass);
            f.rect(wx, wy + 13, 16, 10, glassD); f.vline(wx + 8, wy - 2, wy + 22, stoneL); f.hline(wx, wx + 15, wy + 11, stoneL);
            f.rect(wx - 2, wy + 23, 20, 2, stone);
        }
    }
    f.rect(186, 6, 132, 106, ol); f.rect(187, 7, 130, 105, stone); f.hline(187, 316, 7, stoneL);
    f.poly({ {180, 30}, {324, 30}, {252, 4} }, ol); f.poly({ {186, 28}, {318, 28}, {252, 7} }, stone); f.line(186, 28, 252, 7, stoneL);
    Font::center(f, "UNIVERSITY", 252, 18, C(120, 100, 80));
    for (int k = 0; k < 6; ++k) { int cx = 196 + k * 22; f.rect(cx - 4, 36, 9, 62, ol); f.rect(cx - 3, 36, 7, 62, stoneL); f.vline(cx + 2, 36, 97, stoneD); f.rect(cx - 5, 32, 11, 4, stone); f.rect(cx - 5, 96, 11, 3, stone); }
    f.rect(232, 62, 40, 36, ol); f.rect(233, 63, 38, 35, C(96, 62, 46)); f.vline(252, 63, 97, ol); f.disc(248, 82, 1, gold); f.disc(256, 82, 1, gold);
    f.rect(212, 42, 80, 14, ol); f.rect(213, 43, 78, 12, red); f.hline(213, 290, 43, redL); f.hline(213, 290, 54, redD);
    Font::center(f, "GRADUATION", 252, 46, gold, ol);
    for (int k = 0; k < 4; ++k) { int y = 98 + k * 4; f.rect(176 - k * 6, y, 152 + k * 12, 4, k % 2 ? stoneD : stone); f.hline(176 - k * 6, 327 + k * 6, y, ol); }
    uint8_t pave = C(214, 206, 192), paveD = C(190, 182, 168);
    f.rect(0, 114, 480, 156, pave);
    for (int y = 118, s = 4; y < 270; y += s, s += 1) f.hline(0, 479, y, paveD);
    for (int k = -12; k <= 12; ++k) f.line(252 + k * 22, 114, 252 + k * 58, 269, paveD);
    f.poly({ {234, 114}, {270, 114}, {300, 269}, {204, 269} }, red);
    f.line(234, 114, 204, 269, gold); f.line(270, 114, 300, 269, gold);
    f.line(238, 114, 210, 269, redD); f.line(266, 114, 294, 269, redD); f.line(252, 114, 252, 269, redL);
    static const RGB flag[4] = { {240, 96, 80}, {250, 210, 90}, {110, 170, 240}, {120, 200, 120} };
    for (int side = 0; side < 2; ++side) {
        int x0 = side ? 318 : 0, x1 = side ? 480 : 186;
        for (int x = x0; x < x1; x += 2) f.set(x, 30 + (int)(std::sin((x - x0) * 3.1416f / (x1 - x0)) * 8), ol);
        for (int x = x0 + 6, k = 0; x < x1 - 6; x += 12, ++k) {
            int y = 30 + (int)(std::sin((x - x0) * 3.1416f / (x1 - x0)) * 8), sway = (t / 4 + k) % 2;
            f.poly({ {x - 4, y}, {x + 4, y}, {x + sway, y + 9} }, ol); f.poly({ {x - 3, y + 1}, {x + 3, y + 1}, {x + sway, y + 7} }, C(flag[k % 4]));
        }
    }
}
void UniRound::drawCeremony(Frame& f, int t) {
    int h = t - gradT_ - CEREMONY;
    bool tossed = t - gradT_ >= TOSS;
    static const RGB hair[6] = { {44, 34, 44}, {96, 60, 40}, {30, 26, 30}, {140, 96, 56}, {60, 44, 36}, {20, 20, 24} };
    struct G { int x, feet; bool flip; };
    static const G back[8] = { {204, 152, false}, {326, 152, true}, {352, 152, false}, {380, 152, true}, {408, 152, false}, {436, 152, true}, {462, 152, false}, {222, 152, true} };
    static const G front[6] = { {204, 198, false}, {318, 198, true}, {350, 200, false}, {386, 198, true}, {420, 200, false}, {454, 198, true} };
    auto row = [&](const G* g, int n, int base) {
        for (int i = 0; i < n; ++i) {
            int jump = tossed && t - gradT_ < TOSS + 18 && (h + i) % 6 < 3 ? -2 : 0;
            gradSprite(f, g[i].x, g[i].feet + jump, hair[(i + base) % 6], g[i].flip, tossed ? 1 : 0, !tossed, true);
        }
    };
    row(back, 8, 0);
    row(front, 6, 3);
    gradSprite(f, 262, 252, { 168, 92, 44 }, false, t - gradT_ >= RAISE ? 1 : 0, !tossed, true);
    if (tossed) {
        int a = t - gradT_ - TOSS;
        static const int sx[16] = { 204, 326, 352, 380, 408, 436, 462, 222, 204, 318, 350, 386, 420, 454, 262, 300 };
        for (int k = 0; k < 16; ++k) {
            int y0 = k < 8 ? 114 : k < 14 ? 160 : 210;
            float v = 9.0f + (k % 3), g = 0.3f;
            int y = y0 - (int)(v * a - g * a * a), x = sx[k] + (k % 2 ? 1 : -1) * a / 2;
            if (y > -10 && y < 270) flyingCap(f, x, y, a / 2 + k);
        }
        static const RGB cc[4] = { {240, 96, 80}, {250, 220, 120}, {120, 180, 250}, {250, 250, 250} };
        for (int k = 0; k < 40; ++k) {
            int cx = 190 + (k * 53 + (int)(std::sin((a + k) * 0.3f) * 6)) % 290, cy = (k * 29 + a * (2 + k % 3)) % 260;
            f.rect(cx, cy, 2, 1 + (a + k) % 2, C(cc[k % 4]));
        }
    }
}
