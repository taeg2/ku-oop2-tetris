#include "ui/SoccerRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include <algorithm>
#include <cmath>
static const RGB OL{ 40, 30, 36 };
static const int MAIN_X = 286, MAIN_Y = 250, PAL_X = 404, PAL_Y = 224, KEEP_X = 341, KEEP_Y = 147, GOAL_X = 362, GOAL_Y = 132;
static float ease(float u) { u = std::max(0.0f, std::min(1.0f, u)); return u * u * (3 - 2 * u); }
static int rnd(float v) { return static_cast<int>(std::lround(v)); }
static const Sprite HEAD_SIDE("player head, facing right", {
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
static const Sprite BODY_SIDE("player jersey and shorts, facing right", {
"....JJJJJJJ.....",
"...JJJJJJJJJ....",
"...JJJLJJJJJ....",
"...JJJJWWJJJ....",
"...JJJJJWJJJ....",
"...JJJJJWJJJ....",
"...jjjjjjjjj....",
"...KKKKKKKKK....",
"...KKKKKKKKK....",
"...kKKK.KKKk....",
}, { {'J', {54, 108, 214}}, {'j', {36, 76, 160}}, {'L', {120, 170, 250}}, {'W', {250, 250, 250}}, {'K', {246, 246, 240}}, {'k', {200, 200, 196}} });
struct Look { RGB hair, hairL, jersey, jerseyD; };
static const Look MAIN{ {168, 92, 44}, {206, 128, 64}, {54, 108, 214}, {36, 76, 160} };
static const Look PAL{ {44, 34, 44}, {78, 64, 80}, {54, 108, 214}, {36, 76, 160} };
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
static void player(Frame& f, int cx, int feet, const Look& lk, int pose, bool flip, bool tall = false) {
    static Frame tmp;
    const int W = 22, H = 44;
    uint8_t key = C(255, 0, 255), skin = C(246, 204, 164), skinD = C(214, 164, 128), sock = C(lk.jersey), sockD = C(lk.jerseyD), shoe = C(30, 26, 30), shoeL = C(90, 90, 96);
    tmp.rect(0, 0, W, H, key);
    int ox = 3, oy = 1 + (pose == 4 ? 0 : 0);
    uint8_t trou = C(34, 40, 70), trouD = C(26, 30, 54);
    if (tall) { skin = trou; skinD = trouD; sock = trou; sockD = trouD; }
    auto leg = [&](int hx, int hy, int fx, int fy, bool front) {
        tmp.taper(hx, hy, hx + (fx - hx) / 2, hy + (fy - hy) / 2, 3, 3, front ? skin : skinD);
        tmp.taper(hx + (fx - hx) / 2, hy + (fy - hy) / 2, fx, fy - 1, 3, 3, front ? sock : sockD);
        tmp.rect(fx - 1, fy - 1, 4, 2, shoe); tmp.set(fx + 2, fy - 1, shoeL);
    };
    int hy = oy + 22, fy = oy + 33 + (tall ? 5 : 0);
    if (pose == 3) { leg(ox + 6, hy, ox + 5, fy, false); leg(ox + 9, hy, ox + 15, fy - 4, true); }
    else if (pose == 1) { leg(ox + 6, hy, ox + 3, fy, false); leg(ox + 9, hy, ox + 12, fy - 1, true); }
    else if (pose == 2) { leg(ox + 6, hy, ox + 9, fy - 1, false); leg(ox + 9, hy, ox + 6, fy, true); }
    else if (pose == 4) { leg(ox + 6, hy, ox + 4, fy - 3, false); leg(ox + 9, hy, ox + 11, fy - 3, true); }
    else { leg(ox + 6, hy, ox + 6, fy, false); leg(ox + 9, hy, ox + 9, fy, true); }
    blit(tmp, BODY_SIDE, ox, oy + 12);
    blit(tmp, HEAD_SIDE, ox, oy);
    recolor(tmp, W, H, C(168, 92, 44), C(lk.hair)); recolor(tmp, W, H, C(206, 128, 64), C(lk.hairL));
    int sx = ox + 7, sy = oy + 14, ax = sx, ay = sy + 7;
    if (pose == 1) { ax = sx + 3; ay = sy + 6; } else if (pose == 2) { ax = sx - 3; ay = sy + 6; }
    else if (pose == 3) { ax = sx - 5; ay = sy + 4; } else if (pose == 4) { ax = sx + 2; ay = sy - 8; }
    if (tall) {
        recolor(tmp, W, H, C(54, 108, 214), C(40, 50, 92)); recolor(tmp, W, H, C(36, 76, 160), C(30, 38, 70)); recolor(tmp, W, H, C(120, 170, 250), C(60, 72, 120));
        recolor(tmp, W, H, C(246, 246, 240), C(34, 40, 70)); recolor(tmp, W, H, C(200, 200, 196), C(26, 30, 54));
        tmp.rect(ox + 8, oy + 12, 2, 3, C(250, 250, 250)); tmp.rect(ox + 9, oy + 13, 1, 5, C(190, 50, 60));
    }
    tmp.taper(sx, sy, ax, ay, 3, 3, tall ? C(30, 38, 70) : C(lk.jerseyD)); tmp.disc(ax, ay, 1, C(246, 204, 164));
    outlineCopy(f, tmp, key, W, H, flip ? cx - (W - 1 - (ox + 8)) : cx - (ox + 8), feet - fy, flip);
}
static void studentSprite(Frame& f, int cx, int feet, RGB hair, RGB hairL, bool flip, int step, bool smile, bool bag) {
    static Frame tmp;
    const int W = 24, H = 46;
    uint8_t key = C(255, 0, 255), blazer = C(36, 46, 92), blazerD = C(26, 32, 68), blazerL = C(62, 76, 136), shirt = C(250, 250, 246), tie = C(196, 48, 58), pants = C(110, 112, 122), pantsD = C(84, 86, 96), shoe = C(30, 26, 30), shoeL = C(90, 90, 96), skin = C(246, 204, 164), pack = C(150, 92, 52), packD = C(112, 66, 38), packL = C(186, 124, 76);
    tmp.rect(0, 0, W, H, key);
    int ox = 4, oy = 2, hy = oy + 26, fy = H - 1;
    int s = step % 4, sw = s == 1 ? 3 : s == 3 ? -3 : 0;
    auto leg = [&](int fx, uint8_t c) { tmp.taper(ox + 7, hy, fx, fy - 2, 3, 3, c); tmp.rect(fx - 1, fy - 1, 4, 2, shoe); tmp.set(fx + 2, fy - 1, shoeL); };
    leg(ox + 7 - sw, pantsD);
    leg(ox + 8 + sw, pants);
    if (bag) { tmp.rect(ox - 1, oy + 13, 5, 12, pack); tmp.vline(ox - 1, oy + 13, oy + 24, packD); tmp.hline(ox - 1, ox + 3, oy + 13, packL); tmp.rect(ox, oy + 19, 3, 3, packD); }
    tmp.rect(ox + 3, oy + 12, 10, 15, blazer); tmp.vline(ox + 3, oy + 12, oy + 26, blazerD); tmp.hline(ox + 3, ox + 12, oy + 26, blazerD); tmp.vline(ox + 12, oy + 13, oy + 25, blazerL);
    tmp.poly({ {ox + 9, oy + 12}, {ox + 12, oy + 12}, {ox + 11, oy + 18} }, shirt); tmp.vline(ox + 11, oy + 13, oy + 19, tie); tmp.set(ox + 10, oy + 13, tie);
    if (bag) tmp.line(ox + 4, oy + 13, ox + 6, oy + 22, packD);
    blit(tmp, HEAD_SIDE, ox, oy);
    recolor(tmp, W, H, C(168, 92, 44), C(hair)); recolor(tmp, W, H, C(206, 128, 64), C(hairL));
    if (smile) { tmp.set(ox + 11, oy + 8, C(176, 80, 84)); tmp.set(ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); }
    int sx = ox + 7, sy = oy + 14, ax = sx - sw, ay = sy + 9;
    tmp.taper(sx, sy, ax, ay, 4, 3, blazerD); tmp.disc(ax, ay, 1, skin);
    outlineCopy(f, tmp, key, W, H, flip ? cx - (W - 1 - (ox + 8)) : cx - (ox + 8), feet - fy, flip);
}
static void keeperSprite(Frame& f, int cx, int feet, int pose) {
    uint8_t ol = C(OL), shirt = C(250, 204, 60), shirtD = C(214, 160, 40), shorts = C(30, 30, 40), skin = C(246, 204, 164), glove = C(250, 250, 250), hair = C(52, 36, 30), sock = C(250, 204, 60);
    int x = cx - 7, y = feet - 34 + (pose == 2 ? -10 : 0);
    if (pose == 3) {
        int bx = cx - 16, by = feet - 9;
        f.rect(bx - 1, by - 1, 30, 9, ol); f.rect(bx, by, 14, 7, shirt); f.rect(bx + 14, by + 1, 8, 5, shorts); f.rect(bx + 22, by + 2, 6, 3, sock);
        f.disc(bx - 4, by + 3, 5, ol); f.disc(bx - 4, by + 3, 4, skin); f.rect(bx - 8, by - 1, 7, 3, hair);
        f.rect(bx - 12, by + 5, 5, 3, ol); f.rect(bx - 11, by + 5, 3, 2, glove);
        return;
    }
    f.rect(x + 2, y + 25, 4, 9, ol); f.rect(x + 9, y + 25, 4, 9, ol); f.rect(x + 3, y + 25, 2, 6, skin); f.rect(x + 10, y + 25, 2, 6, skin);
    f.rect(x + 3, y + 30, 2, 3, sock); f.rect(x + 10, y + 30, 2, 3, sock); f.rect(x + 1, y + 33, 5, 2, ol); f.rect(x + 9, y + 33, 5, 2, ol);
    f.rect(x, y + 13, 15, 13, ol); f.rect(x + 1, y + 14, 13, 8, shirt); f.rect(x + 1, y + 21, 13, 4, shorts); f.vline(x + 7, y + 14, y + 21, shirtD);
    int lift = pose == 1 || pose == 2 ? -10 : 0;
    f.taper(x + 1, y + 15, x - 6, y + 19 + lift, 4, 3, ol); f.taper(x + 14, y + 15, x + 21, y + 19 + lift, 4, 3, ol);
    f.taper(x + 1, y + 15, x - 6, y + 19 + lift, 2, 2, shirt); f.taper(x + 14, y + 15, x + 21, y + 19 + lift, 2, 2, shirt);
    f.disc(x - 6, y + 19 + lift, 3, ol); f.disc(x + 21, y + 19 + lift, 3, ol); f.disc(x - 6, y + 19 + lift, 2, glove); f.disc(x + 21, y + 19 + lift, 2, glove);
    f.disc(x + 7, y + 7, 7, ol); f.disc(x + 7, y + 7, 6, skin); f.rect(x + 1, y + 1, 13, 4, hair); f.rect(x + 1, y + 1, 2, 7, hair); f.rect(x + 12, y + 1, 2, 7, hair);
    f.rect(x + 4, y + 7, 2, 2, ol); f.rect(x + 9, y + 7, 2, 2, ol); f.hline(x + 6, x + 8, y + 11, C(176, 80, 84));
}
static void ball(Frame& f, int x, int y, int h, int t) {
    f.ellipse(x, y + 1, 4, 1, C(60, 110, 50));
    int by = y - 3 - h;
    f.disc(x, by, 4, C(OL)); f.disc(x, by, 3, C(250, 250, 250));
    int r = (t / 2) % 2;
    f.set(x - 1 + r, by - 1, C(OL)); f.set(x + 1 - r, by + 1, C(OL)); f.set(x - 2, by + 1 - r, C(OL)); f.set(x + 2, by - 2 + r, C(80, 80, 90));
}
void SoccerRound::reset() { queue_.clear(); curT_ = -1; holder_ = 0; shooter_ = 0; passes_ = 0; saves_ = 0; last_ = 0; goalT_ = -1; drop_ = false; armed_ = false; dropT_ = -100; lines_ = 0; }
void SoccerRound::onLinesCleared(const std::vector<int>&, int totalLines, int) {
    for (int l = lines_ + 1; l <= std::min(totalLines, 10); ++l) {
        if (l == 10) queue_.push_back({ GOAL, l });
        else if (l == 3 || l == 5 || l == 7) queue_.push_back({ BLOCK, l });
        else queue_.push_back({ PASS, l });
    }
    lines_ = totalLines;
}
void SoccerRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t;
    if (cur_.k != PASS) {
        if (cur_.k == GOAL && holder_ == 1) { queue_.push_front(cur_); cur_ = { PASS, cur_.line }; }
        else { shooter_ = holder_; if (cur_.k == BLOCK) armed_ = true; }
    }
}
void SoccerRound::update(const PlayStateReader& g, int t) {
    last_ = t;
    track(g, t);
    if (curT_ >= 0) {
        int a = t - curT_, len = cur_.k == PASS ? PASS_LEN : cur_.k == BLOCK ? BLOCK_LEN : 999;
        if (a >= len) {
            if (cur_.k == PASS) { holder_ = 1 - holder_; ++passes_; }
            else if (cur_.k == BLOCK) { holder_ = 1 - shooter_; ++saves_; }
            curT_ = -1;
        }
    }
    if (curT_ < 0 && !queue_.empty()) start(t);
    if (curT_ >= 0 && cur_.k == GOAL && goalT_ < 0 && t - curT_ >= SHOT_LEN) goalT_ = t;
}
int SoccerRound::pieceTop(const PlayStateReader& g) {
    int y = PlayStateReader::kRows;
    for (auto& p : g.pieceCells()) y = std::min(y, p.second);
    return y;
}
void SoccerRound::track(const PlayStateReader& g, int t) {
    if (armed_ && !g.clearing() && g.hasPiece() && curT_ >= 0 && t - curT_ >= 3) { armed_ = false; drop_ = true; dropT_ = t; dropY_ = pieceTop(g); }
    if (!drop_) return;
    if (g.over() || g.clearing() || !g.hasPiece()) { drop_ = false; return; }
    int y = pieceTop(g);
    if (y < dropY_) drop_ = false;
    else dropY_ = y;
}
void SoccerRound::ballPos(int t, float& x, float& y, float& h) const {
    float mx = MAIN_X + 9, my = MAIN_Y, px = PAL_X - 9, py = PAL_Y;
    x = holder_ == 0 ? mx : px; y = holder_ == 0 ? my : py; h = 0;
    if (goalT_ >= 0) { x = GOAL_X; y = GOAL_Y; h = 6; return; }
    if (curT_ < 0) { h = static_cast<float>((t / 4) % 2); return; }
    int a = t - curT_;
    float fx = holder_ == 0 ? mx : px, fy = holder_ == 0 ? my : py;
    if (cur_.k == PASS) {
        float u = ease(a / static_cast<float>(PASS_LEN)), tx = holder_ == 0 ? px : mx, ty = holder_ == 0 ? py : my;
        x = fx + (tx - fx) * u; y = fy + (ty - fy) * u; h = std::sin(u * 3.1416f) * 6;
        return;
    }
    float gx = cur_.k == GOAL ? GOAL_X : KEEP_X + 8, gy = cur_.k == GOAL ? GOAL_Y : KEEP_Y - 2;
    if (a < SHOT_LEN) { float u = a / static_cast<float>(SHOT_LEN); x = fx + (gx - fx) * u; y = fy + (gy - fy) * u; h = std::sin(u * 3.1416f) * 22 + u * 14; return; }
    float u = ease((a - SHOT_LEN) / static_cast<float>(BOUNCE_LEN)), tx = shooter_ == 0 ? px : mx, ty = shooter_ == 0 ? py : my;
    x = gx + (tx - gx) * u; y = gy + (ty - gy) * u; h = 14 * (1 - u) + std::sin(u * 3.1416f) * 20;
}
void SoccerRound::drawSchool(Frame& f, int t) {
    uint8_t ol = C(OL), wall = C(242, 230, 204), wallL = C(250, 242, 222), wallD = C(218, 202, 170), band = C(206, 186, 150), trim = C(170, 72, 60), trimD = C(126, 50, 44), trimL = C(206, 108, 88);
    uint8_t glass = C(132, 184, 224), glassD = C(100, 150, 200), glassL = C(206, 232, 248), frame = C(250, 250, 244), stone = C(170, 166, 160), stoneD = C(140, 136, 132);
    f.rect(0, 16, 480, 96, wall);
    f.rect(0, 12, 480, 6, ol); f.rect(0, 13, 480, 4, trim); f.hline(0, 479, 13, trimL); f.hline(0, 479, 17, trimD);
    for (int fl = 0; fl < 3; ++fl) { int by = 44 + fl * 26; f.hline(0, 479, by, band); f.hline(0, 479, by + 1, wallD); }
    f.rect(0, 100, 480, 12, stone); f.hline(0, 479, 100, ol); for (int x = 0; x < 480; x += 16) f.vline(x + ((x / 16) % 2) * 8, 101, 111, stoneD); f.hline(0, 479, 106, stoneD);
    auto win = [&](int wx, int wy, int k) {
        f.rect(wx - 1, wy - 1, 18, 18, ol); f.rect(wx, wy, 16, 16, frame);
        f.rect(wx + 2, wy + 2, 12, 12, glass); f.rect(wx + 2, wy + 9, 12, 5, glassD);
        f.line(wx + 3, wy + 7, wx + 7, wy + 3, glassL); f.line(wx + 3, wy + 9, wx + 9, wy + 3, glassL);
        f.vline(wx + 8, wy + 2, wy + 13, frame); f.hline(wx + 2, wx + 13, wy + 8, frame);
        if (k % 5 == 1) { f.rect(wx + 2, wy + 2, 4, 12, C(246, 214, 160)); f.rect(wx + 10, wy + 2, 4, 12, C(246, 214, 160)); }
        if (k % 7 == 3) f.rect(wx + 2, wy + 2, 12, 5, C(236, 236, 226));
        f.rect(wx - 2, wy + 16, 20, 2, ol); f.hline(wx - 1, wx + 16, wy + 16, wallL);
    };
    int k = 0;
    for (int fl = 0; fl < 3; ++fl) for (int c = 0; c < 15; ++c) {
        int wx = 6 + c * 32, wy = 22 + fl * 26;
        if (wx > 196 && wx < 290) continue;
        win(wx, wy, k++);
    }
    f.rect(206, 6, 92, 106, ol); f.rect(207, 7, 90, 105, wallL); f.vline(296, 7, 111, wallD); f.vline(207, 7, 111, C(255, 250, 236));
    f.rect(203, 2, 98, 7, ol); f.rect(204, 3, 96, 5, trim); f.hline(204, 299, 3, trimL);
    const int cy = 9;
    f.disc(252, cy, 10, ol); f.disc(252, cy, 9, trim); f.disc(252, cy, 7, ol); f.disc(252, cy, 6, C(252, 250, 240));
    for (int h = 0; h < 12; h += 3) { float a = h * 0.5236f; f.set(252 + rnd(std::cos(a) * 5), cy + rnd(std::sin(a) * 5), ol); }
    float ha = t * 0.01f - 1.5708f; f.line(252, cy, 252 + rnd(std::cos(ha) * 3), cy + rnd(std::sin(ha) * 3), ol); f.line(252, cy, 252 + rnd(std::cos(ha * 12) * 5), cy + rnd(std::sin(ha * 12) * 5), C(200, 60, 60));
    f.rect(212, 21, 80, 12, ol); f.rect(213, 22, 78, 10, C(36, 70, 56)); f.hline(213, 290, 22, C(60, 100, 80));
    Font::center(f, older(t) ? "HIGH SCHOOL" : "MIDDLE SCHOOL", 252, 24, C(244, 214, 120));
    win(218, 38, 2); win(244, 38, 4); win(270, 38, 6);
    f.rect(218, 62, 68, 38, ol); f.rect(219, 63, 66, 37, C(120, 84, 60));
    for (int d = 0; d < 3; ++d) { int dx = 221 + d * 22; f.rect(dx, 66, 18, 34, ol); f.rect(dx + 1, 67, 16, 33, glass); f.line(dx + 2, 80, dx + 9, 68, glassL); f.vline(dx + 8, 67, 99, frame); f.rect(dx + 6, 82, 2, 4, C(232, 200, 110)); }
    f.poly({ {214, 60}, {290, 60}, {296, 56}, {208, 56} }, ol); f.poly({ {215, 59}, {289, 59}, {293, 57}, {211, 57} }, trim);
    f.rect(210, 100, 84, 4, ol); f.rect(211, 101, 82, 2, C(214, 210, 204)); f.rect(206, 104, 92, 4, ol); f.rect(207, 105, 90, 2, C(200, 196, 190));
}
void SoccerRound::drawField(Frame& f) {
    uint8_t line = C(244, 248, 236), lineF = C(222, 236, 214);
    for (int y = 120; y < 270; ++y) {
        float d = (y - 120) / 150.0f;
        int band = static_cast<int>(std::pow(d + 0.08f, 0.75f) * 8);
        f.hline(0, 479, y, band % 2 ? C(104, 172, 74) : C(118, 186, 84));
    }
    uint8_t fence = C(70, 120, 84), fenceL = C(96, 150, 104), post = C(60, 70, 66);
    f.rect(0, 108, 480, 14, C(46, 96, 64));
    for (int x = 0; x < 480; x += 4) f.vline(x, 109, 121, fence);
    for (int y = 110; y < 122; y += 4) f.hline(0, 479, y, fenceL);
    for (int x = 0; x < 480; x += 40) f.rect(x, 104, 2, 18, post);
    f.hline(0, 479, 107, post); f.hline(0, 479, 122, C(70, 120, 60));
    f.line(150, 138, 479, 138, lineF);
    f.line(150, 138, 76, 269, lineF);
    f.line(262, 138, 236, 186, line); f.line(236, 186, 446, 186, line); f.line(446, 186, 420, 138, line);
    f.line(300, 138, 294, 150, line); f.line(294, 150, 388, 150, line); f.line(388, 150, 382, 138, line);
    f.disc(341, 170, 1, line);
    for (int i = 0; i <= 40; ++i) { float a = 3.1416f * i / 40; int x = 341 + rnd(std::cos(a) * 34), y = 186 + rnd(std::sin(a) * 9); if (y > 186) f.set(x, y, line); }
    f.hline(0, 479, 262, line);
    for (int i = 0; i <= 120; ++i) { float a = 3.1416f + 3.1416f * i / 120; int x = 341 + rnd(std::cos(a) * 74), y = 280 + rnd(std::sin(a) * 30); f.set(x, y, line); f.set(x, y + 1, line); }
    for (int k = 0; k < 40; ++k) { int x = (k * 73) % 470 + 5, y = 140 + (k * 41) % 126; f.set(x, y, C(146, 204, 104)); f.set(x + 1, y - 1, C(146, 204, 104)); }
}
void SoccerRound::drawGoal(Frame& f, int t, bool front) {
    const int L = 296, R = 386, B = 138, T = 104, dx = 6, dy = 10;
    uint8_t post = C(252, 252, 250), postD = C(196, 198, 204), net = C(214, 226, 232), netD = C(176, 192, 200), ol = C(OL);
    int bulge = goalT_ >= 0 && t - goalT_ < 8 ? 3 - std::abs(t - goalT_ - 3) : 0;
    if (!front) {
        int bl = L + dx, br = R - dx, bt = T - dy + 6, bb = B - dy;
        f.poly({ {bl, bt}, {br, bt}, {br, bb}, {bl, bb} }, C(150, 190, 150));
        for (int x = bl; x <= br; x += 4) f.vline(x + (x > 330 && x < 370 ? bulge : 0), bt, bb, net);
        for (int y = bt; y <= bb; y += 4) f.hline(bl, br, y - (y > 110 ? bulge / 2 : 0), net);
        for (int k = 0; k <= 4; ++k) { int y0 = T + (B - T) * k / 4, y1 = bt + (bb - bt) * k / 4; f.line(L, y0, bl, y1, netD); f.line(R, y0, br, y1, netD); }
        for (int k = 0; k <= 2; ++k) { f.line(L + 1, B - k * 2, bl, bb - k * 2, netD); f.line(R - 1, B - k * 2, br, bb - k * 2, netD); }
        for (int x = L; x <= R; x += 6) f.line(x, T, bl + (x - L) * (br - bl) / (R - L), bt, netD);
        f.line(bl, bt, br, bt, postD); f.line(bl, bt, bl, bb, postD); f.line(br, bt, br, bb, postD); f.line(bl, bb, br, bb, netD);
        f.line(L, B, bl, bb, postD); f.line(R, B, br, bb, postD); f.line(L, T, bl, bt, postD); f.line(R, T, br, bt, postD);
        return;
    }
    f.rect(L - 2, T - 2, 4, B - T + 3, ol); f.rect(R - 1, T - 2, 4, B - T + 3, ol); f.rect(L - 2, T - 2, R - L + 5, 4, ol);
    f.vline(L - 1, T - 1, B, post); f.vline(L, T, B, post); f.vline(R, T - 1, B, post); f.vline(R + 1, T, B, postD);
    f.hline(L - 1, R + 1, T - 1, post); f.hline(L, R, T, postD);
    f.hline(L - 1, R + 1, B + 1, C(80, 140, 64));
}
void SoccerRound::drawKeeper(Frame& f, int t) {
    if (older(t)) return;
    int pose = (t / 6) % 2 == 0 ? 0 : 1, x = KEEP_X + static_cast<int>(std::lround(std::sin(t * 0.3f) * 2));
    if (curT_ >= 0 && cur_.k == BLOCK) { int a = t - curT_; if (a >= SHOT_LEN - 3 && a < SHOT_LEN + 6) pose = 2; }
    if (goalT_ >= 0) pose = 3;
    keeperSprite(f, x, KEEP_Y, pose);
}
void SoccerRound::drawPlayers(Frame& f, int t) {
    if (older(t)) {
        int h = t - goalT_ - PAN_DOWN;
        bool look = h > 30 && h < 44;
        player(f, 300, 246, MAIN, h > 46 && h < 54 ? 1 + (h / 2) % 2 : 0, look, true);
        return;
    }
    int pm = (t / 8) % 2 ? 0 : 0, pp = 0;
    bool idle = curT_ < 0;
    int a = curT_ >= 0 ? t - curT_ : 99;
    int kicker = curT_ >= 0 ? (cur_.k == PASS ? holder_ : shooter_) : -1;
    if (kicker >= 0 && a < 3) { if (kicker == 0) pm = 3; else pp = 3; }
    if (idle) { pm = (t / 5) % 2 ? 1 : 0; pp = (t / 5 + 1) % 2 ? 2 : 0; }
    int my = MAIN_Y, py = PAL_Y, mx = MAIN_X, px = PAL_X;
    if (goalT_ >= 0) {
        int g = t - goalT_;
        int jump = (g % 6 < 3) ? -(g % 6) * 3 : -(6 - g % 6) * 3;
        pm = 4; pp = 4; my += jump; py += jump;
        if (g > 16) { mx = MAIN_X + std::min(40, (g - 16) * 3); }
    }
    bool palFlip = !(kicker == 1 && cur_.k != PASS) && goalT_ < 0;
    player(f, px, py, PAL, pp, palFlip);
    player(f, mx, my, MAIN, pm, false);
}
void SoccerRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    f.gradient(0, 0, 480, 16, { {150, 206, 244}, {176, 220, 246} });
    if (older(t)) { drawSchool(f, t); drawPlaza(f, t); Stage2UI::pills(f, t); drawCommonUI(f); return; }
    drawField(f);
    drawSchool(f, t);
    drawGoal(f, t, false);
    Stage2UI::pills(f, t);
    drawCommonUI(f);
}
void SoccerRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    drawKeeper(f, t);
    float bx, by, bh;
    ballPos(t, bx, by, bh);
    bool behind = goalT_ >= 0;
    if (older(t)) { drawFirstDay(f, t); return; }
    if (behind) ball(f, rnd(bx), rnd(by), rnd(bh), t);
    drawGoal(f, t, true);
    drawPlayers(f, t);
    if (!behind) ball(f, rnd(bx), rnd(by), rnd(bh), t);
}
void SoccerRound::drawNext(Frame& f, int piece, int t) {
    if (older(t)) return;
    uint8_t ol = C(OL), panel = C(36, 52, 44);
    int x = 402, y = 30;
    f.rect(x + 14, y + 60, 3, 50, ol); f.rect(x + 56, y + 60, 3, 50, ol); f.vline(x + 15, y + 60, y + 108, C(150, 150, 160)); f.vline(x + 57, y + 60, y + 108, C(150, 150, 160));
    f.rect(x - 2, y - 2, 76, 64, ol); f.rect(x - 1, y - 1, 74, 62, C(200, 200, 206)); f.rect(x + 1, y + 1, 70, 58, panel);
    f.rect(x + 40, y + 24, 28, 32, ol); f.rect(x + 41, y + 25, 26, 30, C(24, 34, 30));
    Font::center(f, "NEXT", x + 54, y + 27, C(250, 220, 120));
    std::vector<std::pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    if (goalT_ < 0 || older(t)) for (auto& c : cs) ToyBlocks::cell6(f, x + 54 - bw / 2 + (c.first - mx) * 6, y + 44 - bh / 2 + (c.second - my) * 6, piece);
}
void SoccerRound::drawHud(Frame& f, int, int t) {
    if (older(t)) return;
    int x = 402, y = 30;
    uint8_t lit = C(250, 220, 120), dim = C(90, 110, 96), red = C(240, 96, 80);
    int score = goalT_ >= 0 && t - goalT_ > 4 && !older(t) ? 1 : 0;
    Font::draw(f, "HOME", x + 4, y + 4, lit); Font::draw(f, "AWAY", x + 46, y + 4, lit);
    Font::center(f, std::to_string(score), x + 15, y + 12, score ? red : lit); Font::center(f, ":", x + 36, y + 12, lit); Font::center(f, "0", x + 57, y + 12, lit);
    int n = older(t) ? 0 : std::min(lines_, 10);
    Font::draw(f, "PASS", x + 4, y + 26, lit);
    for (int i = 0; i < 10; ++i) {
        int bx = x + 4 + (i % 5) * 7, by = y + 36 + (i / 5) * 8;
        bool shot = i == 2 || i == 4 || i == 6 || i == 9;
        f.rect(bx, by, 5, 5, i < n ? (shot ? red : lit) : dim);
    }
}
void SoccerRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    if (goalT_ >= 0) flyToBook(f, t - goalT_ - 6, GOAL_X, GOAL_Y, Stage2UI::ballItem, "THE BALL!", t);
    uint8_t ol = C(OL);
    if (drop_ || t - dropT_ < 6) {
        int bx = boardX(), by = boardY();
        for (int k = 0; k < 3; ++k) { int ax = bx + 16 + k * 24, ay = by - 12 + (t % 3) * 2; f.vline(ax, ay - 6, ay, C(240, 96, 80)); f.line(ax - 3, ay - 3, ax, ay, C(240, 96, 80)); f.line(ax + 3, ay - 3, ax, ay, C(240, 96, 80)); }
        Font::center(f, "DEAD DROP!", bx + 40, by - 24, C(240, 96, 80), ol);
    }
    if (curT_ >= 0 && cur_.k == BLOCK) {
        int a = t - curT_;
        if (a >= SHOT_LEN && a < SHOT_LEN + 8) Font::center(f, "SAVED!", KEEP_X, KEEP_Y - 56, C(250, 220, 120), ol);
    }
    if (goalT_ >= 0) {
        int g = t - goalT_;
        if (g < PAN_UP && (g / 3) % 4 != 3) Font::center(f, "GOAL!!", 341, 160 - std::min(g, 8), C(250, 220, 120), ol);
        for (int k = 0; k < 24 && g < PAN_UP; ++k) {
            int cx = (k * 41 + g * (2 + k % 3)) % 300 + 180, cy = (k * 23 + g * 3) % 150 + 20;
            static const RGB cc[4] = { {240, 96, 80}, {250, 220, 120}, {120, 180, 250}, {250, 250, 250} };
            f.rect(cx, cy, 2, 2, C(cc[k % 4]));
        }
        int p = 0;
        if (g >= PAN_UP && g < LAPSE) p = rnd(270 * ease((g - PAN_UP) / static_cast<float>(LAPSE - PAN_UP)));
        else if (g >= LAPSE && g < PAN_DOWN) p = 270;
        else if (g >= PAN_DOWN && g < HOLD) p = rnd(270 * (1 - ease((g - PAN_DOWN) / static_cast<float>(HOLD - PAN_DOWN))));
        if (p > 0) {
            static Frame sky, copy;
            drawSky(sky, g);
            copy.px = f.px;
            for (int y = 0; y < 270; ++y) {
                if (y < p) std::copy(sky.px.begin() + (270 - p + y) * 480, sky.px.begin() + (271 - p + y) * 480, f.px.begin() + y * 480);
                else std::copy(copy.px.begin() + (y - p) * 480, copy.px.begin() + (y - p + 1) * 480, f.px.begin() + y * 480);
            }
        }
        if (g >= END - 12) { float k = std::min(1.0f, (g - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
}
static float pathX(float y) { return 252 + (y - 113) / 156.0f * 88; }
static void cherryTree(Frame& f, int x, int feet, int t) {
    uint8_t ol = C(OL), bark = C(96, 64, 52), barkD = C(70, 46, 40), p0 = C(226, 140, 170), p1 = C(244, 178, 198), p2 = C(252, 214, 226), p3 = C(198, 108, 142);
    f.taper(x, feet, x - 2, feet - 34, 7, 5, ol); f.taper(x, feet, x - 2, feet - 34, 5, 3, bark); f.vline(x - 2, feet - 30, feet - 2, barkD);
    f.taper(x - 1, feet - 26, x - 14, feet - 40, 3, 2, ol); f.taper(x - 1, feet - 26, x - 14, feet - 40, 2, 1, bark);
    f.taper(x - 1, feet - 22, x + 13, feet - 36, 3, 2, ol); f.taper(x - 1, feet - 22, x + 13, feet - 36, 2, 1, bark);
    static const int bx[9] = { -18, -6, 8, 18, -12, 2, 14, -2, -20 }, by[9] = { -44, -52, -50, -40, -38, -42, -34, -60, -34 }, br[9] = { 10, 11, 10, 9, 9, 10, 8, 9, 7 };
    for (int k = 0; k < 9; ++k) f.disc(x + bx[k], feet + by[k], br[k] + 1, ol);
    for (int k = 0; k < 9; ++k) f.disc(x + bx[k], feet + by[k], br[k], k % 3 == 0 ? p3 : p0);
    for (int k = 0; k < 9; ++k) f.disc(x + bx[k] - 2, feet + by[k] - 2, br[k] - 4, p1);
    for (int k = 0; k < 9; ++k) { f.set(x + bx[k] - 4, feet + by[k] - 4, p2); f.set(x + bx[k] + 1, feet + by[k] - 5 + (t / 6 + k) % 2, p2); }
    f.ellipse(x, feet, 14, 3, C(176, 168, 156));
}
void SoccerRound::drawPlaza(Frame& f, int t) {
    uint8_t ol = C(OL), pave = C(214, 206, 192), paveD = C(190, 182, 168), paveL = C(230, 224, 212), walk = C(236, 228, 214), walkD = C(204, 194, 178), red = C(176, 44, 52), redD = C(136, 30, 40), redL = C(210, 76, 80), gold = C(232, 188, 72), grass = C(118, 176, 84), grassD = C(96, 152, 70), stone = C(170, 166, 160), stoneD = C(132, 128, 124), stoneL = C(200, 196, 190);
    f.rect(0, 112, 480, 158, pave);
    for (int y = 116, s = 4; y < 270; y += s, s += 1) f.hline(0, 479, y, paveD);
    for (int k = -12; k <= 12; ++k) f.line(252 + k * 22, 112, 252 + k * 58, 269, paveD);
    f.hline(0, 479, 112, ol); f.hline(0, 479, 113, paveL);
    f.poly({ {380, 160}, {479, 160}, {479, 269}, {400, 269} }, grass); for (int k = 0; k < 30; ++k) { int gx = 392 + (k * 37) % 86, gy = 166 + (k * 23) % 100; f.vline(gx, gy, gy + 2, grassD); f.set(gx + 1, gy, grassD); }
    f.poly({ {380, 160}, {479, 160}, {479, 162}, {381, 162} }, ol);
    f.poly({ {236, 113}, {268, 113}, {380, 269}, {300, 269} }, walk);
    for (int y = 120, s = 5; y < 270; y += s, s += 1) { float c = pathX(static_cast<float>(y)), w = 16 + (y - 113) / 156.0f * 24; f.hline(static_cast<int>(c - w), static_cast<int>(c + w), y, walkD); }
    f.line(236, 113, 300, 269, ol); f.line(268, 113, 380, 269, ol);
    f.rect(212, 34, 80, 14, ol); f.rect(213, 35, 78, 12, red); f.hline(213, 290, 35, redL); f.hline(213, 290, 46, redD);
    f.poly({ {213, 47}, {219, 47}, {216, 51} }, red); f.poly({ {285, 47}, {291, 47}, {288, 51} }, red);
    Font::center(f, "WELCOME", 252, 38, gold, ol);
    auto pot = [&](int x) {
        static const RGB fl[3] = { {240, 96, 80}, {250, 210, 90}, {244, 178, 198} };
        f.rect(x - 7, 100, 14, 12, ol); f.rect(x - 6, 101, 12, 10, C(150, 96, 62)); f.hline(x - 6, x + 5, 101, C(186, 126, 84));
        for (int k = 0; k < 7; ++k) { int fx = x - 7 + (k * 5) % 15, fy = 90 + (k * 3) % 8; f.disc(fx, fy, 3, ol); f.disc(fx, fy, 2, C(fl[k % 3])); f.set(fx, fy, C(252, 246, 220)); }
        f.rect(x - 8, 96, 16, 4, C(70, 130, 70));
    };
    pot(208); pot(296);
    cherryTree(f, 214, 150, t);
    cherryTree(f, 444, 176, t);
    auto pillar = [&](int x, bool plate) {
        f.rect(x - 9, 214, 18, 56, ol); f.rect(x - 8, 215, 16, 55, stone); f.vline(x - 8, 215, 269, stoneL); f.vline(x + 7, 215, 269, stoneD);
        for (int y = 224; y < 270; y += 10) f.hline(x - 8, x + 7, y, stoneD);
        f.rect(x - 11, 208, 22, 7, ol); f.rect(x - 10, 209, 20, 5, stoneL); f.hline(x - 10, x + 9, 213, stoneD);
        if (plate) { f.rect(x - 7, 228, 14, 30, ol); f.rect(x - 6, 229, 12, 28, C(52, 44, 40)); for (int k = 0; k < 4; ++k) f.hline(x - 4, x + 3, 233 + k * 6, gold); }
    };
    pillar(290, true); pillar(392, false);
}
void SoccerRound::drawFirstDay(Frame& f, int t) {
    int h = t - goalT_ - PAN_DOWN;
    struct A { int x, feet; RGB hair; bool flip; int step; bool smile; bool bag; };
    std::vector<A> as;
    static const RGB hair[6] = { {44, 34, 44}, {96, 60, 40}, {30, 26, 30}, {140, 96, 56}, {60, 44, 36}, {20, 20, 24} };
    auto lit = [](RGB c) { return RGB{ std::min(255, c.r + 30), std::min(255, c.g + 24), std::min(255, c.b + 20) }; };
    for (int k = 0; k < 5; ++k) {
        int a = (h + k * 16) % 80;
        float y = 276 - a * 3.0f;
        if (y < 118 || y > 280) continue;
        int side = k % 2 ? -14 : -4;
        as.push_back({ static_cast<int>(pathX(y)) + side, static_cast<int>(y), hair[k % 6], true, a / 2, false, true });
    }
    as.push_back({ 204, 238, hair[3], false, 0, true, true });
    as.push_back({ 228, 238, hair[2], true, 0, (t / 10) % 3 == 0, true });
    as.push_back({ 412, 214, hair[5], false, 0, true, true });
    as.push_back({ 436, 214, hair[1], true, 0, (t / 12) % 2 == 0, true });
    float my = 262;
    int mstep = 0;
    if (h >= 46) { my = 262 - (h - 46) * 3.5f; mstep = (h - 46) / 2; }
    if (my >= 118) as.push_back({ static_cast<int>(pathX(my)) + 14, static_cast<int>(my), RGB{ 168, 92, 44 }, true, mstep, h >= 30, true });
    std::sort(as.begin(), as.end(), [](const A& p, const A& q) { return p.feet < q.feet; });
    for (auto& a : as) studentSprite(f, a.x, a.feet, a.hair, lit(a.hair), a.flip, a.step, a.smile, a.bag);
    uint8_t pk = C(244, 178, 198), pkL = C(252, 214, 226);
    for (int k = 0; k < 36; ++k) {
        int px = (k * 53 + t * (1 + k % 2) + static_cast<int>(std::sin((t + k * 7) * 0.2f) * 5)) % 480, py = (k * 31 + t * (2 + k % 3)) % 270;
        if (px > 90 && px < 190 && py > 50 && py < 230) continue;
        f.set(px, py, k % 3 ? pk : pkL); if (k % 2) f.set(px + 1, py, pk);
    }
}

void SoccerRound::drawSky(Frame& f, int g) {
    float u = std::max(0.0f, std::min(1.0f, (g - LAPSE) / static_cast<float>(PAN_DOWN - LAPSE)));
    float ph = 6.2832f * 7 * u * u;
    float night = (1 - std::cos(ph)) / 2, dusk = std::max(0.0f, std::sin(ph * 2)) * (1 - std::abs(night - 0.5f) * 2) * 0.8f;
    auto mixc = [](RGB a, RGB b, float k) { return RGB{ rnd(a.r + (b.r - a.r) * k), rnd(a.g + (b.g - a.g) * k), rnd(a.b + (b.b - a.b) * k) }; };
    RGB top = mixc(mixc({ 110, 180, 240 }, { 12, 18, 48 }, night), { 120, 80, 150 }, dusk * 0.5f);
    RGB mid = mixc(mixc({ 150, 206, 244 }, { 28, 36, 80 }, night), { 240, 140, 110 }, dusk);
    RGB bot = mixc(mixc({ 176, 220, 246 }, { 44, 52, 100 }, night), { 250, 190, 130 }, dusk);
    f.gradient(0, 0, 480, 270, { top, mid, bot });
    for (int k = 0; k < 70; ++k) {
        int x = (k * 137) % 480, y = (k * 71) % 250;
        float tw = night - 0.35f - ((k * 13) % 7) * 0.04f;
        if (tw > 0) f.set(x, y, C(mixc(bot, { 255, 255, 240 }, std::min(1.0f, tw * 2))));
        if (tw > 0.4f && k % 5 == 0) { f.set(x + 1, y, C(200, 210, 240)); f.set(x - 1, y, C(200, 210, 240)); f.set(x, y + 1, C(200, 210, 240)); f.set(x, y - 1, C(200, 210, 240)); }
    }
    float sa = 3.1416f + ph, ma = ph;
    int sx = 240 + rnd(std::cos(sa) * 230), sy = 300 + rnd(std::sin(sa) * 250), mx = 240 + rnd(std::cos(ma) * 230), my = 300 + rnd(std::sin(ma) * 250);
    if (sy < 290) {
        uint8_t ray = C(mixc({ 255, 236, 150 }, bot, 0.4f));
        for (int k = 0; k < 8; ++k) { float a = k * 0.785f + ph * 0.2f; f.line(sx + rnd(std::cos(a) * 18), sy + rnd(std::sin(a) * 18), sx + rnd(std::cos(a) * 25), sy + rnd(std::sin(a) * 25), ray); }
        f.disc(sx, sy, 14, C(255, 214, 90)); f.disc(sx, sy, 12, C(255, 236, 140)); f.disc(sx - 4, sy - 4, 4, C(255, 250, 210));
    }
    if (my < 290) {
        f.disc(mx, my, 12, C(236, 236, 214)); f.disc(mx + 5, my - 3, 11, C(mixc(mid, top, 0.5f)));
        f.disc(mx - 5, my + 3, 2, C(206, 206, 186)); f.disc(mx - 2, my - 5, 1, C(206, 206, 186));
    }
    float speed = 1 + u * u * 30;
    for (int k = 0; k < 6; ++k) {
        int cx = (static_cast<int>(k * 97 + g * speed * (1 + k % 3))) % 600 - 60, cy = 30 + k * 36;
        RGB cc = mixc({ 252, 252, 252 }, { 70, 80, 120 }, night);
        int len = std::min(60, static_cast<int>(speed * 1.5f));
        f.ellipse(cx, cy, 18 + len / 2, 5, C(cc)); f.ellipse(cx + 10, cy - 4, 10 + len / 3, 5, C(cc));
    }
}
