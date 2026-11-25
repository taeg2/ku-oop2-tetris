#include "ui/InterviewRound.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include <algorithm>
#include <cmath>
static const RGB OL{ 40, 30, 36 };
static const int MAIN_X = 288, FLOOR_Y = 214, EX_X = 424;
static int rnd(float v) { return static_cast<int>(std::lround(v)); }
static const Sprite HEAD_SIDE("interview head, facing right", {
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
struct Seat { RGB hair, hairL, suit, suitD, tie; bool glasses; };
static const Seat HERO{ {168, 92, 44}, {206, 128, 64}, {44, 52, 86}, {32, 38, 64}, {190, 50, 60}, false };
static const Seat BOSS{ {150, 150, 158}, {196, 196, 204}, {70, 70, 82}, {52, 52, 62}, {60, 90, 160}, true };
static void recolor(Frame& tmp, int w, int h, uint8_t from, uint8_t to) { for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) if (tmp.get(i, j) == from) tmp.set(i, j, to); }
static void seated(Frame& f, int cx, int floor, const Seat& lk, bool flip, int mood, int arm, int t) {
    static Frame tmp;
    const int W = 34, H = 36, S = 2;
    uint8_t key = C(255, 0, 255), ol = C(OL), suit = C(lk.suit), suitD = C(lk.suitD), shirt = C(246, 246, 242), tie = C(lk.tie), pants = C(lk.suitD), shoe = C(30, 26, 30), shoeL = C(80, 76, 84), skin = C(246, 204, 164), chair = C(50, 50, 58), chairL = C(84, 84, 96), metal = C(150, 150, 160), paper = C(252, 250, 240);
    tmp.rect(0, 0, W, H, key);
    int ox = 5, oy = 2, fy = H - 1;
    tmp.rect(ox - 2, oy + 8, 4, 16, chair); tmp.vline(ox - 1, oy + 9, oy + 22, chairL);
    tmp.rect(ox, oy + 23, 18, 2, chair); tmp.hline(ox, ox + 17, oy + 23, chairL);
    tmp.vline(ox + 8, oy + 25, fy - 2, metal); tmp.hline(ox + 2, ox + 14, fy - 1, metal); tmp.set(ox + 2, fy, ol); tmp.set(ox + 14, fy, ol); tmp.set(ox + 8, fy, ol);
    int tap = mood == 1 && t % 4 < 2 ? 1 : 0;
    tmp.rect(ox + 4, oy + 19, 15, 4, pants);
    tmp.rect(ox + 16, oy + 21 - tap, 3, fy - oy - 23, pants);
    tmp.rect(ox + 15, fy - 2 - tap, 7, 2, shoe); tmp.hline(ox + 16, ox + 21, fy - 2 - tap, shoeL);
    tmp.rect(ox + 2, oy + 11, 9, 11, suit); tmp.vline(ox + 2, oy + 11, oy + 21, suitD);
    tmp.poly({ {ox + 8, oy + 11}, {ox + 11, oy + 11}, {ox + 10, oy + 17} }, shirt); tmp.vline(ox + 10, oy + 12, oy + 18, tie); tmp.set(ox + 9, oy + 12, tie);
    blit(tmp, HEAD_SIDE, ox, oy);
    recolor(tmp, W, H, C(168, 92, 44), C(lk.hair)); recolor(tmp, W, H, C(206, 128, 64), C(lk.hairL));
    if (lk.glasses) { tmp.hline(ox + 7, ox + 13, oy + 5, ol); tmp.rect(ox + 10, oy + 4, 3, 3, ol); tmp.set(ox + 11, oy + 5, C(200, 220, 240)); }
    if (mood == 2) { tmp.set(ox + 11, oy + 4, ol); tmp.set(ox + 11, oy + 5, C(252, 252, 252)); tmp.set(ox + 12, oy + 5, ol); tmp.rect(ox + 10, oy + 8, 2, 2, ol); tmp.set(ox + 11, oy + 8, C(250, 250, 250)); }
    if (mood == 3) { tmp.set(ox + 11, oy + 5, C(246, 204, 164)); tmp.hline(ox + 10, ox + 12, oy + 5, ol); tmp.set(ox + 11, oy + 4, ol); tmp.hline(ox + 10, ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); tmp.set(ox + 9, oy + 7, C(240, 160, 150)); }
    if (mood == 4 && (t / 2) % 2) { tmp.set(ox + 11, oy + 8, ol); tmp.set(ox + 12, oy + 9, ol); }
    int sx = ox + 6, sy = oy + 13, ax = ox + 20, ay = oy + 18;
    if (arm == 1) { ax = ox + 13 + (t % 3) - 1; ay = oy + 1 + (t % 2) * 2; }
    if (arm == 2) { ax = ox + 19; ay = oy + 14 - (t % 6 < 3 ? 1 : 0); }
    tmp.taper(sx, sy, ax, ay, 3, 3, suitD); tmp.rect(ax - 1, ay - 1, 3, 2, skin);
    if (arm == 2) { tmp.rect(ax - 2, ay - 7, 6, 8, paper); tmp.hline(ax - 1, ax + 2, ay - 5, C(160, 170, 190)); tmp.hline(ax - 1, ax + 1, ay - 3, C(160, 170, 190)); }
    int dx = flip ? cx - (W - 1 - (ox + 8)) * S : cx - (ox + 8) * S, dy = floor - fy * S;
    f.ellipse(cx + (flip ? -4 : 4), floor + 1, 22, 3, C(92, 100, 114));
    for (int j = -1; j <= H; ++j) for (int i = -1; i <= W; ++i) {
        int si = flip ? W - 1 - i : i;
        uint8_t c = (si >= 0 && si < W && j >= 0 && j < H) ? tmp.get(si, j) : key;
        bool edge = false;
        if (c == key) for (int k = 0; k < 4 && !edge; ++k) { int ni = si + (k == 0) - (k == 1), nj = j + (k == 2) - (k == 3); if (ni >= 0 && ni < W && nj >= 0 && nj < H && tmp.get(ni, nj) != key) edge = true; }
        if (c == key && !edge) continue;
        f.rect(dx + i * S, dy + j * S, S, S, c == key ? ol : c);
    }
}
static const char* QUESTIONS[20] = { "INTRODUCE YOURSELF", "WHY OUR COMPANY?", "YOUR STRENGTHS?", "YOUR WEAKNESS?", "IN 5 YEARS?", "TEAMWORK?", "BIGGEST FAILURE?", "WHY HIRE YOU?", "SALARY?", "ANY PRESSURE?", "LEADERSHIP?", "A HARD DECISION?", "WHY THIS FIELD?", "A CONFLICT?", "YOUR DREAM?", "WORK OVERTIME?", "YOUR HOBBIES?", "DESCRIBE YOURSELF", "LAST WORDS?", "ANY QUESTIONS?" };
static const char* BABBLES[4] = { "UGA BUGA", "UHH... UM...", "UGA UGA BUGA", "I... UHH..." };
void InterviewRound::reset() { pieceNo_ = 0; lastTop_ = -1; odd_ = false; nextOdd_ = false; queue_.clear(); cur_ = 0; curT_ = -1; last_ = 0; passT_ = -1; lines_ = 0; shown_ = 0; lineT_ = 0; }
void InterviewRound::onLinesCleared(const std::vector<int>&, int totalLines, int) {
    while (lines_ < totalLines && lines_ < 20) queue_.push_back(++lines_);
}
void InterviewRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t; shown_ = cur_; lineT_ = t;
    if (cur_ == 20) passT_ = t;
}
void InterviewRound::update(const PlayStateReader& g, int t) {
    last_ = t;
    if (passT_ >= 0) return;
    if (!g.hasPiece()) lastTop_ = -1;
    else {
        int top = PlayStateReader::kRows;
        for (auto& c : g.pieceCells()) top = std::min(top, c.second);
        if (lastTop_ < 0 || top < lastTop_) { ++pieceNo_; odd_ = nextOdd_ || shown_ >= 7; nextOdd_ = shown_ >= 7; }
        lastTop_ = top;
    }
    if (babbling(t)) { odd_ = true; nextOdd_ = true; }
    if (curT_ >= 0 && t - curT_ < Q_LEN && queue_.empty()) return;
    if (curT_ >= 0 && t - curT_ < 6) return;
    if (!queue_.empty()) start(t);
}
bool InterviewRound::blank(int t) const { return shown_ >= 10 && passT_ < 0 && t % 80 >= 54; }
void InterviewRound::bubble(Frame& f, int x, int y, int w, const char* text, bool tailRight, uint8_t ink) {
    uint8_t ol = C(OL), paper = C(252, 250, 244);
    f.rect(x - 1, y - 1, w + 2, 13, ol); f.rect(x, y, w, 11, paper); f.hline(x + 1, x + w - 2, y + 10, C(222, 218, 206));
    int tx = tailRight ? x + w - 10 : x + 6;
    f.poly({ {tx, y + 11}, {tx + 6, y + 11}, {tailRight ? tx + 8 : tx - 2, y + 16} }, ol);
    f.poly({ {tx + 1, y + 10}, {tx + 5, y + 10}, {tailRight ? tx + 6 : tx, y + 14} }, paper);
    Font::center(f, text, x + w / 2, y + 3, ink);
}
void InterviewRound::drawClock(Frame& f, int t) {
    uint8_t ol = C(OL), face = C(250, 248, 238), rim = C(120, 86, 60), hand = C(40, 34, 40), red = C(210, 60, 60);
    int cx = 226, cy = 44;
    f.disc(cx, cy, 15, ol); f.disc(cx, cy, 14, rim); f.disc(cx, cy, 11, face);
    for (int k = 0; k < 12; ++k) f.set(cx + rnd(std::cos(k * 0.5236f) * 9), cy + rnd(std::sin(k * 0.5236f) * 9), ol);
    float speed = shown_ >= 4 ? 0.9f : 0.08f;
    float a1 = t * speed * 0.08f - 1.57f, a2 = t * speed - 1.57f;
    f.line(cx, cy, cx + rnd(std::cos(a1) * 5), cy + rnd(std::sin(a1) * 5), hand);
    f.line(cx, cy, cx + rnd(std::cos(a2) * 8), cy + rnd(std::sin(a2) * 8), shown_ >= 4 ? red : hand);
    f.set(cx, cy, red);
    if (shown_ >= 4 && passT_ < 0 && (t / 3) % 2) Font::center(f, "HURRY", cx, cy + 19, red, ol);
}
void InterviewRound::drawRoom(Frame& f, int t) {
    uint8_t ol = C(OL), wall = C(226, 222, 210), wallD = C(204, 198, 184), wallL = C(238, 234, 224), wain = C(150, 110, 78), wainD = C(116, 82, 58), wainL = C(178, 136, 98), carpet = C(110, 118, 132), carpetD = C(92, 100, 114), carpetL = C(128, 136, 150);
    f.rect(0, 0, 480, FLOOR_Y, wall);
    for (int x = 0; x < 480; x += 24) f.vline(x, 0, 150, wallD);
    f.hline(0, 479, 0, wallL);
    f.rect(0, 150, 480, FLOOR_Y - 150, wain); f.hline(0, 479, 150, ol); f.hline(0, 479, 151, wainL);
    for (int x = 6; x < 480; x += 40) { f.rect(x, 158, 32, 48, wainD); f.rect(x + 1, 159, 30, 46, wain); f.hline(x + 1, x + 30, 159, wainL); }
    f.rect(0, FLOOR_Y, 480, 270 - FLOOR_Y, carpet); f.hline(0, 479, FLOOR_Y, ol); f.hline(0, 479, FLOOR_Y + 1, carpetL);
    for (int k = 0; k < 90; ++k) f.set((k * 53) % 480, FLOOR_Y + 3 + (k * 29) % 54, k % 2 ? carpetD : carpetL);
    uint8_t frame = C(236, 236, 232), sky = C(150, 200, 238), skyL = C(196, 226, 246), city = C(120, 140, 170), cityD = C(96, 114, 144), cityW = C(206, 222, 238);
    int wx = 282, wy = 22, ww = 120, wh = 82;
    f.rect(wx - 3, wy - 3, ww + 6, wh + 6, ol); f.rect(wx - 2, wy - 2, ww + 4, wh + 4, frame);
    f.gradient(wx, wy, ww, wh, { {132, 190, 236}, {176, 214, 242}, {214, 232, 248} });
    static const int bh[10] = { 40, 58, 30, 66, 48, 36, 54, 26, 62, 44 };
    for (int k = 0; k < 10; ++k) {
        int bx = wx + k * 12, top = wy + wh - bh[k];
        f.rect(bx, top, 12, bh[k], k % 2 ? city : cityD);
        for (int yy = top + 4; yy < wy + wh - 2; yy += 6) for (int xx = bx + 2; xx < bx + 10; xx += 4) f.rect(xx, yy, 2, 3, cityW);
    }
    f.ellipse(wx + 30 + (t / 8) % 80, wy + 14, 12, 3, C(250, 252, 255));
    (void)sky; (void)skyL;
    f.vline(wx + ww / 2, wy, wy + wh - 1, frame); f.hline(wx, wx + ww - 1, wy + wh / 2, frame);
    f.rect(wx - 6, wy + wh + 2, ww + 12, 4, ol); f.rect(wx - 5, wy + wh + 3, ww + 10, 2, frame);
    f.rect(414, 30, 50, 34, ol); f.rect(415, 31, 48, 32, C(40, 64, 110)); f.disc(431, 47, 8, C(250, 196, 70)); f.disc(431, 47, 5, C(40, 64, 110)); f.rect(442, 42, 16, 3, C(240, 240, 236)); f.rect(442, 48, 12, 3, C(240, 240, 236));
    uint8_t pot = C(176, 110, 70), potD = C(136, 82, 52), leaf = C(70, 140, 80), leafD = C(48, 104, 60), leafL = C(110, 180, 110);
    int px = 466, py = 150;
    for (int k = 0; k < 7; ++k) { float a = -2.6f + k * 0.35f; int lx = px + rnd(std::cos(a) * 20), ly = py - 26 + rnd(std::sin(a) * 26); f.taper(px, py - 18, lx, ly, 3, 1, ol); f.taper(px, py - 18, lx, ly, 2, 1, k % 2 ? leaf : leafD); f.set(lx, ly, leafL); }
    f.rect(px - 9, py - 18, 18, 18, ol); f.rect(px - 8, py - 17, 16, 17, pot); f.vline(px + 6, py - 17, py - 1, potD); f.hline(px - 9, px + 8, py - 18, ol);
    drawClock(f, t);
}
void InterviewRound::drawExaminer(Frame& f, int t) {
    bool talk = asking(t) || (passT_ >= 0 && t - passT_ < END - 12);
    seated(f, EX_X, FLOOR_Y, BOSS, true, talk ? 4 : 0, 2, t);
}
void InterviewRound::drawTable(Frame& f) {
    uint8_t ol = C(OL), top = C(156, 108, 72), topL = C(186, 134, 92), leg = C(120, 80, 54), legD = C(92, 60, 42), paper = C(252, 250, 240), glass = C(206, 230, 246), water = C(150, 200, 240);
    int x0 = 306, x1 = 396, y = 183;
    f.rect(x0 - 1, y - 1, x1 - x0 + 2, 7, ol); f.rect(x0, y, x1 - x0, 5, top); f.hline(x0, x1 - 1, y, topL);
    f.rect(x0 + 6, y + 5, 6, FLOOR_Y - y - 5, ol); f.rect(x0 + 7, y + 5, 4, FLOOR_Y - y - 6, leg); f.vline(x0 + 10, y + 5, FLOOR_Y - 2, legD);
    f.rect(x1 - 12, y + 5, 6, FLOOR_Y - y - 5, ol); f.rect(x1 - 11, y + 5, 4, FLOOR_Y - y - 6, leg); f.vline(x1 - 8, y + 5, FLOOR_Y - 2, legD);
    f.ellipse((x0 + x1) / 2, FLOOR_Y + 1, 40, 3, C(92, 100, 114));
    f.rect(x0 + 30, y - 3, 22, 3, ol); f.rect(x0 + 31, y - 2, 20, 2, paper);
    f.rect(x0 + 56, y - 10, 8, 10, ol); f.rect(x0 + 57, y - 9, 6, 9, glass); f.rect(x0 + 57, y - 6, 6, 6, water); f.set(x0 + 58, y - 8, C(250, 252, 255));
}
void InterviewRound::drawMain(Frame& f, int t) {
    bool passed = passT_ >= 0, relaxed = passed && t - passT_ >= SMILE, panic = babbling(t);
    int shake = panic ? ((t % 2) ? 1 : -1) : 0;
    int mood = relaxed ? 3 : panic ? 2 : 1, arm = panic ? 1 : 0;
    seated(f, MAIN_X + shake, FLOOR_Y, HERO, false, mood, arm, t);
    uint8_t sweat = C(150, 200, 250), sweatL = C(220, 240, 255);
    int hx = MAIN_X + shake + 6, hy = FLOOR_Y - 66;
    if (!relaxed) {
        int drops = shown_ >= 10 ? 3 : shown_ >= 4 ? 2 : 1;
        for (int k = 0; k < drops; ++k) {
            int sx = hx - 10 + k * 7, sy = hy + 2 + ((t + k * 5) % 12);
            f.set(sx, sy, sweatL); f.rect(sx - 1, sy + 1, 3, 2, sweat); f.set(sx, sy + 3, sweat);
        }
    }
    if (relaxed) for (int k = 0; k < 6; ++k) {
        int a = (t - passT_ + k * 4) % 24, sx = hx + rnd(std::cos(k * 1.05f) * (18 + a / 3)), sy = hy + 10 + rnd(std::sin(k * 1.05f) * (14 + a / 4));
        if (a < 12) { f.set(sx, sy, C(255, 236, 140)); f.set(sx - 1, sy, C(255, 250, 210)); f.set(sx + 1, sy, C(255, 250, 210)); f.set(sx, sy - 1, C(255, 250, 210)); f.set(sx, sy + 1, C(255, 250, 210)); }
    }
}
static const std::vector<std::pair<int, int>> WEIRD[6] = {
    { {1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2} },
    { {0, 0}, {2, 0}, {1, 1}, {0, 2}, {2, 2} },
    { {0, 0}, {1, 0} },
    { {1, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {0, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {0, 0}, {1, 1}, {2, 2} },
};
static const std::vector<std::pair<int, int>>& weirdShape(int k) { return WEIRD[((k % 6) + 6) % 6]; }
static void paperCell(Frame& f, int x, int y, int type) {
    const RGB* r = ToyBlocks::ramp(type);
    uint8_t ol = C(r[0]), tab = C(r[2]), tabD = C(r[1]), paper = C(mix(r[4], RGB{ 252, 250, 242 }, 0.55f)), line = C(r[3]), fold = C(mix(r[3], RGB{ 230, 224, 210 }, 0.5f));
    f.rect(x, y, 8, 8, ol);
    f.rect(x + 1, y + 1, 6, 6, paper);
    f.rect(x + 1, y + 1, 6, 2, tab); f.hline(x + 1, x + 6, y + 2, tabD);
    f.hline(x + 2, x + 5, y + 4, line); f.hline(x + 2, x + 4, y + 5, line);
    f.set(x + 6, y + 6, fold); f.set(x + 5, y + 6, fold); f.set(x + 6, y + 5, fold);
}
void InterviewRound::drawPaperBlocks(Frame& f, const PlayStateReader& g, int t) {
    int bx = boardX(), by = boardY();
    uint8_t cell = C(22, 26, 42), line = C(32, 38, 58);
    bool hide = blank(t);
    std::vector<int> skip;
    if (g.clearing()) skip = g.clearingRows();
    for (int y = 0; y < PlayStateReader::kRows; ++y) {
        if (std::find(skip.begin(), skip.end(), y) != skip.end()) continue;
        for (int x = 0; x < PlayStateReader::kCols; ++x) {
            if (g.at(x, y) < 0) continue;
            if (hide) {
                int px = bx + x * 8, py = by + y * 8;
                f.rect(px, py, 8, 8, cell); if (x) f.vline(px, py, py + 7, line); if (y) f.hline(px, px + 7, py, line);
                if ((x + y + t / 2) % 5 == 0) f.set(px + 3, py + 4, C(60, 66, 96));
                continue;
            }
            paperCell(f, bx + x * 8, by + y * 8, g.over() ? 7 : g.at(x, y));
        }
    }
    if (!babbling(t) && !odd_) {
        for (auto& c : g.pieceCells()) if (c.second >= 0) paperCell(f, bx + c.first * 8, by + c.second * 8, g.current());
        return;
    }
    auto erase = [&](int x, int y) {
        if (x < 0 || x >= PlayStateReader::kCols || y < 0 || y >= PlayStateReader::kRows || g.at(x, y) >= 0) return;
        int px = bx + x * 8, py = by + y * 8;
        f.rect(px, py, 8, 8, cell); if (x) f.vline(px, py, py + 7, line); if (y) f.hline(px, px + 7, py, line);
    };
    for (auto& c : g.ghostCells()) erase(c.first, c.second);
    for (auto& c : g.pieceCells()) erase(c.first, c.second);
    int mx = 99, my = 99;
    for (auto& c : g.pieceCells()) { mx = std::min(mx, c.first); my = std::min(my, c.second); }
    if (mx == 99) return;
    bool flick = babbling(t);
    const auto& w = weirdShape(flick ? cur_ + t / 4 : pieceNo_ * 7 + 3);
    for (auto& c : w) {
        int x = std::min(PlayStateReader::kCols - 1, mx + c.first), y = my + c.second;
        if (y >= 0 && y < PlayStateReader::kRows) paperCell(f, bx + x * 8, by + y * 8, flick ? (g.current() + t / 4) % 7 : (pieceNo_ * 5 + 1) % 7);
    }
}
void InterviewRound::warpBoard(Frame& f, int t) {
    int bx = boardX(), by = boardY();
    static std::vector<uint8_t> row(80);
    for (int y = by; y < by + 160; ++y) {
        int dx = rnd(std::sin(y * 0.18f + t * 0.5f) * 1.6f);
        if (!dx) continue;
        for (int x = 0; x < 80; ++x) row[x] = f.get(bx + x, y);
        for (int x = 0; x < 80; ++x) f.set(bx + x, y, row[std::max(0, std::min(79, x - dx))]);
    }
}
void InterviewRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    drawRoom(f, t);
    Stage2UI::pills(f, t);
    drawCommonUI(f);
}
void InterviewRound::drawForeground(Frame& f, const PlayStateReader& g, int t) {
    drawPaperBlocks(f, g, t);
    drawExaminer(f, t);
    drawMain(f, t);
    drawTable(f);
}
void InterviewRound::drawNext(Frame& f, int piece, int t) {
    uint8_t ol = C(OL), paper = C(250, 248, 240), paperD = C(222, 218, 206), metal = C(150, 150, 160), metalD = C(110, 110, 120), ink = C(60, 60, 80);
    int x = 202, y = 84;
    f.line(x + 26, y + 70, x + 8, FLOOR_Y, ol); f.line(x + 27, y + 70, x + 9, FLOOR_Y, metalD);
    f.line(x + 26, y + 70, x + 44, FLOOR_Y, ol); f.line(x + 27, y + 70, x + 45, FLOOR_Y, metalD);
    f.line(x + 26, y + 70, x + 26, FLOOR_Y - 4, metal);
    f.rect(x - 1, y - 4, 56, 6, ol); f.rect(x, y - 3, 54, 4, metal);
    f.rect(x - 1, y + 1, 56, 70, ol); f.rect(x, y + 2, 54, 68, paper); f.hline(x, x + 53, y + 69, paperD);
    f.rect(x + 3, y + 66, 48, 3, C(244, 240, 226));
    if (passT_ >= 0) { Font::center(f, "PASS", x + 27, y + 26, C(60, 150, 80)); f.rect(x + 8, y + 22, 38, 1, C(60, 150, 80)); f.rect(x + 8, y + 36, 38, 1, C(60, 150, 80)); return; }
    Font::center(f, "NEXT", x + 27, y + 5, ink);
    bool odd = babbling(t) || nextOdd_;
    std::vector<std::pair<int, int>> cs = babbling(t) ? weirdShape(cur_ + t / 4 + 3) : nextOdd_ ? weirdShape((pieceNo_ + 1) * 7 + 3) : SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    int bw = (Mx - mx + 1) * 8, bh = (My - my + 1) * 8;
    for (auto& c : cs) paperCell(f, x + 27 - bw / 2 + (c.first - mx) * 8, y + 26 - bh / 2 + (c.second - my) * 8, babbling(t) ? (piece + t / 4 + 2) % 7 : odd ? ((pieceNo_ + 1) * 5 + 1) % 7 : piece);
    (void)t;
}
void InterviewRound::drawHud(Frame& f, int, int t) {
    uint8_t ink = C(60, 60, 80), done = C(60, 150, 80), todo = C(214, 208, 192), red = C(210, 60, 60);
    int x = 202, y = 84;
    Font::draw(f, "Q", x + 6, y + 40, ink);
    Font::draw(f, std::to_string(std::min(20, shown_ + (passT_ >= 0 ? 0 : 1))) + "/20", x + 16, y + 40, ink);
    for (int i = 0; i < 20; ++i) { int cx = x + 7 + (i % 5) * 9, cy = y + 49 + (i / 5) * 5; f.rect(cx, cy, 6, 3, i < shown_ ? done : i == 3 || i == 6 || i == 9 || i == 14 ? red : todo); }
    (void)t;
}
void InterviewRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    if (passT_ >= 0) flyToBook(f, t - passT_ - WELCOME, EX_X - 30, 170, Stage2UI::idItem, "JOB ID!", t);
    uint8_t ol = C(OL), ink = C(50, 50, 70), panicInk = C(200, 60, 60);
    if (shown_ >= 7 && passT_ < 0) warpBoard(f, t);
    if (passT_ < 0 && curT_ >= 0) {
        int a = t - curT_;
        if (a < ASK) { const char* q = QUESTIONS[(cur_ - 1) % 20]; int w = Font::width(q) + 10; bubble(f, std::min(472 - w, EX_X - w / 2), 128, w, q, false, ink); }
        else if (a < Q_LEN) { const char* b = BABBLES[(cur_ + a / 6) % 4]; int w = Font::width(b) + 10; bubble(f, std::max(196, MAIN_X - w / 2 + ((t % 2) ? 1 : 0)), 128, w, b, true, panicInk); }
    }
    int bx = boardX(), by = boardY();
    if (passT_ < 0) {
        if (shown_ >= 4 && t - lineT_ < 14 && shown_ < 7) Font::center(f, "FASTER", bx + 40, by - 18, C(250, 220, 120), ol);
        if (shown_ >= 7 && shown_ < 10 && t - lineT_ < 14) Font::center(f, "WEIRD SHAPES", bx + 40, by - 18, C(200, 160, 250), ol);
        if (blank(t)) Font::center(f, "MIND BLANK", bx + 40, by - 18, C(240, 240, 250), ol);
        if (shown_ >= 15 && t % 100 >= 60 && t % 100 < 64 && (t / 2) % 2) Font::center(f, "!!", MAIN_X, 134, C(250, 90, 80), ol);
    }
    if (passT_ >= 0) {
        int a = t - passT_;
        if (a < WELCOME) bubble(f, EX_X - 50, 128, Font::width("YOU PASSED!") + 10, "YOU PASSED!", false, C(60, 150, 80));
        else if (a < END - 12) bubble(f, EX_X - 60, 128, Font::width("WELCOME ABOARD") + 10, "WELCOME ABOARD", false, C(60, 150, 80));
        if (a >= SMILE && a < SMILE + 14) Font::center(f, "PHEW...", MAIN_X, 134, C(250, 250, 240), ol);
        if (a >= END - 12) { float k = std::min(1.0f, (a - END + 13) / 12.0f); for (auto& c : palette()) c = RGB{ rnd(c.r * (1 - k)), rnd(c.g * (1 - k)), rnd(c.b * (1 - k)) }; }
    }
    if (whiteout(t) > 0) {
        int w = whiteout(t);
        uint8_t white = C(250, 250, 246), grey = C(190, 190, 196);
        for (int y = 0; y < 270; ++y) for (int x = 0; x < 480; ++x) {
            bool on = w >= 3 || (w == 2 ? (x + y) % 4 != 0 : (x + y) % 2 == 0);
            if (on) f.set(x, y, white);
        }
        if (w >= 3) Font::center(f, ". . .", 240, 130, grey);
    }
}
int InterviewRound::whiteout(int t) const {
    if (shown_ < 15 || passT_ >= 0) return 0;
    int a = t % 100;
    if (a < 64 || a >= 84) return 0;
    if (a < 66 || a >= 82) return 1;
    if (a < 68 || a >= 80) return 2;
    return 3;
}
