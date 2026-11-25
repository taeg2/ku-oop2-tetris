#include "ui/SessionPlayState.h"
#include "ui/BeachRound.h"
#include "ui/People.h"
#include "ui/PixelFont.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
using namespace std;

namespace {
const RGB OUT{ 42, 26, 28 };
const RGB SKY[5] = { {116,100,170}, {160,112,168}, {222,128,140}, {248,162,120}, {254,204,132} };
const RGB SAND_HL{ 252, 226, 184 }, SAND_L{ 238, 202, 152 }, SAND{ 218, 174, 122 }, SAND_D{ 180, 136, 92 }, SAND_DD{ 134, 96, 68 };
const RGB WET{ 176, 138, 110 }, WATER_L{ 236, 176, 150 }, TREE{ 70, 56, 92 }, TREE2{ 96, 70, 106 };
const RGB SKIN{ 246, 204, 164 };
const RGB EYE{ 34, 26, 34 };
const RGB TENT{ 226, 120, 150 }, TENT_D{ 176, 80, 112 }, TENT_L{ 246, 170, 190 }, SUCK{ 255, 214, 214 };
const RGB GOO{ 24, 18, 30 }, GOO_L{ 70, 60, 90 };
const int HORIZON = 88, SHORE = 132;
const int BKT_X = 2, BKT_Y = 130, OCTO_X = 312, OCTO_Y = 196;
class Spot { public: int x, y, v; };
const Spot SPOTS[5] = { {130,180,0}, {232,180,4}, {326,180,1}, {280,236,2}, {150,236,3} };
const int SHAPE[5][6] = { {10,10,-4,26,20,26}, {12,20,-6,30,26,30}, {28,28,-21,18,34,32}, {11,11,-4,36,22,36}, {11,11,-4,20,22,20} };
const float UP_SEQ[10] = { 0.6f, 0.2f, 0.0f, 0.0f, 0.0f, 0.3f, 0.8f, 1.1f, 1.0f, 1.0f };
const int PATS = 4, PAT_LEN = 2, HAPPY = 3, STAND = 2, KNEEL = 2;
const float SPEED = 6.0f, PI = 3.14159265f;
enum { P_KNEEL = 0, P_BUILD = 8, P_HAPPY = 88, P_STANDUP = 90, P_RUN_R = 92, P_RUN_L = 96, P_END = 100, P_WAVE = 102 };
int wave(int t, float period, float amp, float phase = 0) {
    return static_cast<int>(lround(sin(t * 6.2832f / period + phase) * amp));
}
float clamp01(float v) { return max(0.0f, min(1.0f, v)); }
float ease(float t) { t = clamp01(t); return 0.5f - 0.5f * cos(PI * t); }
int rnd(float v) { return static_cast<int>(lround(v)); }
const int BAYER[4][4] = { {0,8,2,10}, {12,4,14,6}, {3,11,1,9}, {15,7,13,5} };
void blendPixel(Frame& f, int x, int y, RGB c, float a) {
    if (x < 0 || y < 0 || x >= Frame::W || y >= Frame::H) return;
    f.set(x, y, C(mix(palette()[f.get(x, y)], c, a)));
}
void blendEllipse(Frame& f, float x0, float y0, float x1, float y1, RGB c, float a) {
    float cx = (x0 + x1) / 2, cy = (y0 + y1) / 2, rx = (x1 - x0) / 2, ry = (y1 - y0) / 2;
    for (int y = static_cast<int>(y0); y <= static_cast<int>(y1); ++y) for (int x = static_cast<int>(x0); x <= static_cast<int>(x1); ++x) {
        float u = (x - cx) / rx, v = (y - cy) / ry;
        if (u * u + v * v <= 1) blendPixel(f, x, y, c, a);
    }
}
void sparkle(Frame& f, int x, int y) {
    f.set(x, y, C(255, 255, 220));
    f.set(x - 1, y, C(250, 222, 110)); f.set(x + 1, y, C(250, 222, 110));
    f.set(x, y - 1, C(250, 222, 110)); f.set(x, y + 1, C(250, 222, 110));
}
void tower(Frame& f, int x0, int y0, int w, int h, bool crenel, int windows, bool door, int slump) {
    int x1 = x0 + w - 1;
    uint8_t s = C(SAND), sl = C(SAND_L), sd = C(SAND_D), win = C(110, 70, 46);
    f.poly({ {x0 + 1, y0}, {x1 - 1, y0}, {x1, y0 + h}, {x0, y0 + h} }, s);
    f.line(x1 - 1, y0 + 1, x1, y0 + h, sl); f.line(x0 + 1, y0 + 1, x0, y0 + h, sd);
    if (crenel) for (int cx = x0 + 1; cx < x1; cx += 4) { f.rect(cx, y0 - 3, 2, 4, s); f.set(cx + 1, y0 - 3, sl); }
    if (slump) f.ellipse((x0 + x1) / 2, y0, (x1 - x0) / 2, slump, s);
    for (int k = 0; k < windows; ++k) {
        int wx = x0 + w / 2 - 1 + static_cast<int>((k - (windows - 1) / 2.0f) * 5);
        f.rect(wx, y0 + 4, 2, 2, win);
    }
    if (door) {
        int dx = x0 + w / 2 - 2;
        f.rect(dx, y0 + h - 6, 5, 7, C(96, 60, 40)); f.hline(dx + 1, dx + 3, y0 + h - 7, C(96, 60, 40));
        f.vline(dx - 1, y0 + h - 7, y0 + h, sd);
    }
}
void topper(Frame& f, int x, int y, int kind, int color) {
    if (kind == 0) {
        f.vline(x, y - 9, y, C(120, 84, 60));
        const RGB* r = ToyBlocks::ramp(color);
        f.poly({ {x + 1, y - 9}, {x + 7, y - 7}, {x + 1, y - 5} }, C(r[2])); f.line(x + 1, y - 5, x + 7, y - 7, C(r[0]));
    } else if (kind == 1) {
        f.poly({ {x - 4, y - 7}, {x + 4, y - 7}, {x + 3, y}, {x - 3, y} }, C(130, 30, 40));
        f.poly({ {x - 3, y - 6}, {x + 3, y - 6}, {x + 2, y - 1}, {x - 2, y - 1} }, C(214, 58, 62));
        f.hline(x - 3, x + 2, y - 6, C(246, 120, 110));
    } else if (kind == 2) {
        f.ellipse(x, y - 1, 4, 3, C(176, 140, 130)); f.ellipse(x, y - 1, 3, 2, C(250, 226, 220)); f.hline(x - 4, x + 4, y + 1, C(SAND));
        f.vline(x, y - 3, y, C(214, 180, 176));
    } else {
        f.ellipse(x, y - 2, 4, 3, C(176, 140, 40)); f.ellipse(x, y - 2, 3, 2, C(250, 214, 70));
        f.disc(x + 3, y - 6, 3, C(176, 140, 40)); f.disc(x + 3, y - 6, 2, C(250, 214, 70));
        f.hline(x + 6, x + 7, y - 6, C(240, 140, 60)); f.set(x + 3, y - 7, C(40, 30, 30));
    }
}
const Sprite KID_LEGS_KNEEL("kid legs kneeling", {
    "...OBBBOOBBBO...",
    "..OSSSOOOOSSSO..",
    ".OKSSSSOOSSSSKO.",
    ".OOOOOOOOOOOOOO.",
}, peopleColors());
const Sprite KID_BODY_ONE_ARM("kid body, right arm free", {
    "....OOYYYYOO....",
    "..OOYYYYYYYOO...",
    ".OYYOYYYYYYyO...",
    ".OYYOYYYYYYyO...",
    ".OSSOYYYYYYyO...",
    ".OSSOYYYYYYyO...",
    ".OSSOyyyyyyyO...",
    "..OOOBBBBBBO....",
    "....OBBbbBBO....",
}, peopleColors());
}
void BeachRound::drawSky(Frame& f, int t) {
    for (int y = 0; y < HORIZON; ++y) {
        float v = static_cast<float>(y) / HORIZON * 4;
        int i = min(3, static_cast<int>(v)); float fr = v - i;
        for (int x = 0; x < 480; ++x) f.set(x, y, C(SKY[(fr * 16 > BAYER[y % 4][x % 4]) ? i + 1 : i]));
    }
    f.disc(360, 78, 12, C(255, 232, 170)); f.disc(360, 78, 8, C(255, 244, 206));
    int drift = (t / 6) % 560;
    for (int k = 0; k < 3; ++k) {
        int cx = (60 + k * 190 + drift) % 560 - 40, cy = 16 + k * 7;
        uint8_t c = C(250, 220, 210), u = C(222, 168, 168);
        f.ellipse(cx, cy + 3, 16, 3, u); f.ellipse(cx - 6, cy, 7, 4, c); f.ellipse(cx + 3, cy - 2, 9, 6, c); f.ellipse(cx + 11, cy + 1, 6, 4, c);
    }
}
void BeachRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    drawSky(f, t);
    for (int x = 0; x < 480; ++x) {
        int h = static_cast<int>(7 + 4 * sin(x * 0.03f) + 2 * sin(x * 0.11f));
        f.vline(x, HORIZON - h, HORIZON, C(TREE2));
    }
    for (int k = 0; k < 40; ++k) {
        int tx = (k * 97 + 13) % 480, th = 6 + (k * 7) % 8;
        f.poly({ {tx, HORIZON - th - 6}, {tx - 3, HORIZON - 1}, {tx + 3, HORIZON - 1} }, C(TREE));
    }
    const RGB LAKE[6] = { {170,130,176}, {160,124,172}, {150,118,168}, {140,112,164}, {130,106,160}, {122,100,156} };
    for (int y = HORIZON; y < SHORE; ++y) f.hline(0, 479, y, C(LAKE[min(5, (y - HORIZON) * 6 / (SHORE - HORIZON))]));
    for (int y = HORIZON + 1; y < SHORE - 2; y += 2) {
        int w = 3 + (y - HORIZON) / 3;
        for (int k = 0; k < 2; ++k) { int x = 360 + ((y * 37 + k * 53) % (2 * w + 1)) - w; f.hline(x - 3, x + 4, y, C(WATER_L)); }
    }
    for (int k = 0; k < 26; ++k) { int x = (k * 73) % 470, y = HORIZON + 4 + (k * 17) % (SHORE - HORIZON - 8); f.hline(x, x + 3 + k % 5, y, C(186, 150, 190)); }
    f.rect(0, SHORE, 480, 270 - SHORE, C(SAND));
    for (int y = SHORE; y < SHORE + 7; ++y) f.hline(0, 479, y, y < SHORE + 4 ? C(WET) : C(196, 152, 110));
    int foam = SHORE + static_cast<int>(1.5f + 1.5f * sin(t * 6.2832f / 40));
    f.hline(0, 479, foam, C(236, 220, 214));
    for (int x = 0; x < 480; x += 7) f.set(x + (t / 4) % 7, foam + 1, C(236, 220, 214));
    for (int k = 0; k < 70; ++k) {
        int x = (k * 131) % 468, y = SHORE + 10 + (k * 37) % (270 - SHORE - 14), w = 6 + k % 9;
        f.hline(x, x + w, y, C(SAND_D)); f.hline(x + 1, x + w - 1, y - 1, C(SAND_L));
    }
    for (int k = 0; k < 140; ++k) {
        int x = (k * 53) % 478, y = SHORE + 8 + (k * 29) % (270 - SHORE - 10);
        f.hline(x, x + 1, y, (k % 5 < 3) ? C(SAND_L) : C(SAND_D));
    }
    int shells[4][2] = { {96,150}, {418,262}, {250,262}, {34,200} };
    for (auto& s : shells) { f.ellipse(s[0] + 3, s[1] + 3, 3, 2, C(SAND_DD)); f.ellipse(s[0] + 3, s[1] + 3, 2, 1, C(250, 232, 222)); f.set(s[0] + 3, s[1] + 2, C(224, 196, 190)); }
    for (int i = 0; i < 5; ++i) {
        float b = build(i);
        if (b <= 0) continue;
        int x = SPOTS[i].x, y = SPOTS[i].y, w = SHAPE[SPOTS[i].v][4], len = static_cast<int>(SHAPE[SPOTS[i].v][3] * b);
        f.poly({ {x - w + 4, y}, {x + 2, y}, {x - len - 6, y - 4}, {x - len - w, y - 3} }, C(196, 150, 104));
    }
    if (goneT_ < 0) blendEllipse(f, kx_ - 12, ky_ - 3, kx_ + 12, ky_ + 2, RGB{ 150, 104, 80 }, 0.35f);
    drawCommonUI(f);
}
void BeachRound::drawBoardFrame(Frame& f, int t) {
    int bx = boardX(), by = boardY();
    f.ellipse(bx + 40, by + 186, 76, 20, C(SAND_DD));
    f.ellipse(bx + 40, by + 184, 74, 18, C(SAND));
    f.ellipse(bx + 50, by + 180, 50, 10, C(SAND_L));
    f.ellipse(bx + 40, by + 178, 54, 8, C(SAND_D));
    RoundScene::drawBoardFrame(f, t);
    int cx = bx + 40;
    tower(f, cx - 20, by - 26, 40, 20, true, 3, false, 0);
    f.ellipse(cx, by - 7, 26, 3, C(SAND_D));
    topper(f, cx + 12, by - 30, 0, 4 + (t / 30) % 2 * 0);
    f.set(cx + 18 + wave(t, 12, 1.0f), by - 37, C(250, 250, 250));
    for (int x = bx - 8; x < bx + 89; x += 3) f.ellipse(x, by + 166 + (x % 5) / 2, 3, 2, C(SAND));
}
void BeachRound::drawCastle(Frame& f, int i, float b, int t) {
    (void)t;
    int cx = SPOTS[i].x, base = SPOTS[i].y, v = SPOTS[i].v, mw = SHAPE[v][4];
    if (b <= 0) return;
    f.ellipse(cx, base - 1, mw, 4, C(SAND_DD)); f.ellipse(cx, base - 1, mw - 1, 3, C(SAND)); f.hline(cx - mw + 4, cx + mw - 4, base - 4, C(SAND_L));
    Frame tmp;
    uint8_t clear = C(255, 0, 255);
    for (int y = base - 50; y <= base + 2; ++y) for (int x = cx - 40; x <= cx + 40; ++x) tmp.set(x, y, clear);
    switch (v) {
    case 0: tower(tmp, cx - 10, base - 26, 20, 26, true, 2, false, 0); break;
    case 1: tower(tmp, cx + 6, base - 16, 14, 16, true, 1, false, 0); tower(tmp, cx - 12, base - 30, 18, 30, true, 1, false, 0); break;
    case 2: tower(tmp, cx - 28, base - 18, 13, 18, true, 1, false, 0); tower(tmp, cx + 15, base - 18, 13, 18, true, 1, false, 0);
            tower(tmp, cx - 15, base - 32, 30, 32, true, 3, true, 0); break;
    case 3: tower(tmp, cx - 11, base - 22, 22, 22, true, 2, true, 0); tower(tmp, cx - 7, base - 36, 14, 14, true, 1, false, 0); break;
    default: tower(tmp, cx - 11, base - 20, 22, 20, false, 2, false, 4); break;
    }
    for (int k = 0; k < 26; ++k) {
        int x = cx - 24 + (k * 17 + i * 5) % 48, y = base - 40 + (k * 11) % 38;
        if (tmp.get(x, y) == C(SAND) && tmp.get(x + 1, y) == C(SAND)) tmp.hline(x, x + 1, y, (k % 2) ? C(SAND_HL) : C(SAND_D));
    }
    int top = static_cast<int>(base + 2 - 54 * b);
    uint8_t edge = C(112, 76, 54);
    for (int y = max(top, base - 50); y <= base; ++y)
        for (int x = cx - 40; x <= cx + 40; ++x) {
            uint8_t c = tmp.get(x, y);
            if (c == clear) {
                bool n = false;
                for (int d = 0; d < 4 && !n; ++d) {
                    int nx = x + (d == 0) - (d == 1), ny = y + (d == 2) - (d == 3);
                    uint8_t q = tmp.get(nx, ny);
                    n = q != clear && q != 0 && ny >= top && ny <= base + 2 && abs(nx - cx) <= 40 && ny >= base - 50;
                }
                if (n) f.set(x, y, edge);
            } else if (c != 0 || (y >= base - 50)) {
                if (c != 0) f.set(x, y, c);
            }
        }
    if (b >= 1.0f) {
        static const int KIND[5] = { 0, 0, 3, 1, 2 };
        static const int TOPX[5] = { 0, -3, 6, 0, 0 }, TOPY[5] = { 29, 33, 35, 39, 24 };
        topper(f, cx + TOPX[v], base - TOPY[v], KIND[v], (i * 3 + 2) % 7);
    }
}
void BeachRound::drawCastleGoo(Frame& f, int i, int k) {
    int cx = SPOTS[i].x, base = SPOTS[i].y, mw = SHAPE[SPOTS[i].v][4], h = SHAPE[SPOTS[i].v][3];
    uint8_t goo = C(GOO), gl = C(GOO_L);
    for (int y = base - h - 6; y <= base; ++y)
        for (int x = cx - 30; x <= cx + 30; ++x)
            if (f.get(x, y) == C(110, 70, 46) || f.get(x, y) == C(96, 60, 40)) f.set(x, y, goo);
    for (int d = 0; d < 6; ++d) {
        int dx = cx - mw / 2 + d * mw / 5 + (d % 2), start = d * 3;
        int len = max(0, min(h + 4, (k - start) * 2));
        if (len <= 0) continue;
        int top = base - h + 2 + (d % 3) * 4;
        f.vline(dx, top, min(base, top + len), goo);
        if (d % 2 == 0) f.vline(dx + 1, top, min(base, top + len - 3), goo);
        if (top + len < base) f.disc(dx, top + len, 1, goo);
        f.set(dx - 1, top + 1, gl);
    }
    int pr = min(mw + 6, k * 2);
    if (k > 6) { f.ellipse(cx, base + 1, pr, max(1, pr / 6), goo); f.hline(cx - pr / 2, cx - pr / 3, base, gl); }
}
void BeachRound::drawKid(Frame& f, int t) {
    if (goneT_ >= 0) return;
    using namespace People;
    int x = rnd(kx_), y = rnd(ky_), p = pose_;
    uint8_t ol = C(OUT), skin = C(SKIN), eye = C(EYE), mouth = C(176, 80, 84);
    auto arm = [&](int sx, int sy, int hx, int hy) {
        f.taper(sx, sy, hx, hy, 5, 4, ol); f.taper(sx, sy, hx, hy, 3, 2, skin); f.disc(hx, hy, 2, ol); f.disc(hx, hy, 1, skin);
    };
    auto happyEyes = [&](int ox, int oy) {
        for (int ex : { 3, 11 }) {
            f.rect(ox + ex, oy + 8, 2, 2, skin);
            f.set(ox + ex, oy + 9, eye); f.set(ox + ex + 1, oy + 8, eye); f.set(ox + ex + 2, oy + 9, eye);
        }
        f.rect(ox + 7, oy + 11, 2, 2, mouth);
    };
    auto lookSide = [&](int ox, int oy, int dir) {
        for (int ex : { 3, 11 }) {
            f.rect(ox + ex, oy + 8, 2, 2, skin);
            f.set(ox + ex + (dir > 0 ? 1 : 0), oy + 8, eye); f.set(ox + ex + (dir > 0 ? 1 : 0), oy + 9, eye);
        }
    };
    if (p >= P_RUN_R && p < P_END) {
        People::drawKid(f, x, y, p < P_RUN_L ? RIGHT : LEFT, p % 4);
        return;
    }
    bool kneel = p < P_STANDUP;
    int ox = x - 8, top = kneel ? y - 28 : y - 32;
    if (kneel) blit(f, KID_LEGS_KNEEL, ox, y - 4);
    else blit(f, KID_LEGS_F, ox, y - 8);
    bool freeArm = (p >= P_BUILD && p < P_HAPPY) || p >= P_WAVE;
    blit(f, freeArm ? KID_BODY_ONE_ARM : KID_BODY_F, ox, top + 15);
    blit(f, KID_HEAD_F, ox, top);
    if (p >= P_BUILD && p < P_HAPPY) {
        int k = (p - P_BUILD) % 8, dx, h; patReach(cur_, dx, h);
        int hx = x + dx, hy = y - rnd(h * build(cur_)) - 2 - (k % 2 == 0 ? 6 : 0);
        arm(ox + 12, top + 17, hx, hy);
        f.rect(ox + 7, top + 11, 2, 1, mouth);
    } else if (p >= P_WAVE) {
        int k = p - P_WAVE;
        arm(ox + 12, top + 17, ox + 19 + (k < 2 ? 2 : -1), top + 2);
        happyEyes(ox, top);
    } else if (p >= P_HAPPY && p < P_STANDUP) happyEyes(ox, top);
    else if (p == P_END) lookSide(ox, top, -1);
    else if (p == P_END + 1) { lookSide(ox, top, -1); f.rect(ox + 7, top + 11, 2, 2, mouth); }
    else if (t % 43 == 0) for (int ex : { 3, 11 }) { f.hline(ox + ex, ox + ex + 1, top + 8, skin); f.hline(ox + ex, ox + ex + 1, top + 9, eye); }
}
void BeachRound::drawBucket(Frame& f, int t) {
    const int x = BKT_X, y = BKT_Y;
    uint8_t ol = C(OUT), blue = C(60, 130, 210), blueD = C(40, 96, 170), blueL = C(120, 180, 240);
    f.ellipse(x + 32, y + 54, 30, 4, C(SAND_D));
    f.poly({ {x + 9, y + 6}, {x + 55, y + 6}, {x + 61, y + 52}, {x + 3, y + 52} }, ol);
    f.poly({ {x + 10, y + 7}, {x + 54, y + 7}, {x + 60, y + 51}, {x + 4, y + 51} }, blue);
    f.poly({ {x + 46, y + 7}, {x + 54, y + 7}, {x + 60, y + 51}, {x + 51, y + 51} }, blueD);
    f.line(x + 12, y + 8, x + 8, y + 50, blueL);
    f.rect(x + 2, y + 47, 61, 6, ol); f.rect(x + 3, y + 48, 59, 4, blueD); f.hline(x + 3, x + 61, y + 48, blueL);
    f.ellipse(x + 32, y + 6, 23, 3, ol); f.ellipse(x + 32, y + 6, 22, 2, blueD);
    static const int SX[10] = { 18, 32, 46, 13, 26, 39, 52, 18, 32, 46 }, SY[10] = { 17, 17, 17, 29, 29, 29, 29, 41, 41, 41 };
    int n = min(lines_, 10), from = n - clearN_;
    for (int i = 0; i < n; ++i) {
        int a = (i >= from) ? t - clearT_ : 99;
        int s = (a == 0) ? 6 : (a == 1) ? 5 : 4;
        int cx = x + SX[i], cy = y + SY[i];
        uint8_t st = C(244, 128, 92), sd = C(196, 84, 64);
        for (int k = 0; k < 5; ++k) {
            float ang = -1.5708f + k * 1.2566f;
            f.taper(cx, cy, cx + rnd(cos(ang) * s), cy + rnd(sin(ang) * s), 3, 1, st);
        }
        f.set(cx, cy, sd); f.set(cx + 1, cy + 1, sd);
        if (a < 3) sparkle(f, cx + 5, cy - 5);
    }
    for (int i = n; i < 10; ++i) f.set(x + SX[i], y + SY[i], C(90, 150, 220));
}
void BeachRound::drawTentacle(Frame& f, int x0, int y0, int x1, int y1, int bend, float w0, int t, float black) {
    RGB base = mix(TENT, GOO, black), dark = mix(TENT_D, GOO, black), lite = mix(TENT_L, GOO_L, black);
    uint8_t ol = C(OUT), c = C(base), cd = C(dark), cl = C(lite), sk = C(mix(SUCK, GOO_L, black));
    const int N = 14;
    int px[N + 1], py[N + 1];
    for (int i = 0; i <= N; ++i) {
        float u = static_cast<float>(i) / N;
        float cx = (x0 + x1) / 2.0f + bend, cy = (y0 + y1) / 2.0f;
        px[i] = static_cast<int>((1 - u) * (1 - u) * x0 + 2 * (1 - u) * u * cx + u * u * x1) + static_cast<int>(sin(t * 0.3f + u * 4) * u * 1.5f);
        py[i] = static_cast<int>((1 - u) * (1 - u) * y0 + 2 * (1 - u) * u * cy + u * u * y1);
    }
    for (int i = 0; i < N; ++i) { float w = w0 - (w0 - 2) * i / N; f.taper(px[i], py[i], px[i + 1], py[i + 1], w + 2, w + 1.5f, ol); }
    for (int i = 0; i < N; ++i) { float w = w0 - (w0 - 2) * i / N; f.taper(px[i], py[i], px[i + 1], py[i + 1], w, w - 0.5f, c); }
    for (int i = 0; i < N; ++i) { f.set(px[i] - 1, py[i] - 1, cl); f.set(px[i] + 1, py[i] + 1, cd); }
    for (int i = 2; i < N; i += 2) f.set(px[i] + (bend > 0 ? -2 : 2), py[i], sk);
}
void BeachRound::drawOcto(Frame& f, int t) {
    const int HX = OCTO_X + 24, HY = OCTO_Y + 54;
    int fr = t - startT_;
    float black = goneT_ >= 0 ? clamp01((t - goneT_ - 10) / 12.0f) : 0;
    f.ellipse(HX, HY, 18, 5, C(SAND_DD)); f.ellipse(HX, HY, 14, 3, C(70, 46, 34));
    if (goneT_ >= 0 && t >= goneT_ + 16) {
        float r = min(16.0f, (t - goneT_ - 16) * 0.8f);
        f.ellipse(HX, HY + 1, 14 + rnd(r * 0.6f), 4 + rnd(r * 0.15f), C(GOO));
    }
    if (goneT_ >= 0) {
        int tw = ((t / 3) % 7 == 0) ? 2 : 0;
        drawTentacle(f, HX - 6, HY, HX - 10, HY - 40, -8, 6, t, black);
        drawTentacle(f, HX + 6, HY, HX + 12 + tw, HY - 46, 10, 6, t, black);
        return;
    }
    if (endT_ >= 0 && t >= endT_ + 18) {
        int e = t - endT_;
        drawTentacle(f, HX - 6, HY, HX - 10, HY - 40, -8, 6, t, 0);
        drawTentacle(f, HX + 6, HY, HX + 14 + wave(e, 6, 7.0f), HY - 48, 10, 6, t, 0);
        return;
    }
    float up;
    int kNew = t - clearT_;
    if (fr < 6) up = 0;
    else if (fr < 11) up = ease((fr - 6) / 5.0f);
    else if (kNew >= 0 && kNew < 10) up = UP_SEQ[kNew];
    else up = 1.0f;
    int h = rnd(46 * up);
    if (h < 4) {
        if (fr % 3 == 0) for (int k = 0; k < 3; ++k) f.set(HX - 6 + k * 6, HY - 3 - k % 2, C(SAND_HL));
        return;
    }
    int sway = wave(t, 30, 2.0f), tipY = HY - h;
    drawTentacle(f, HX - 6, HY, HX - 11 + sway, tipY, -10, 6, t, 0);
    drawTentacle(f, HX + 6, HY, HX + 11 + sway, tipY, 10, 6, t, 0);
    if (up > 0.6f && endT_ < 0) {
        int p = (kNew >= 2 && kNew < 5) ? prevPiece_ : piece_;
        auto cs = SessionPlayState::shapeOf(p);
        int mx = 9, my = 9, Mx = 0, My = 0;
        for (auto& c : cs) { mx = min(mx, c.first); my = min(my, c.second); Mx = max(Mx, c.first); My = max(My, c.second); }
        int pw = (Mx - mx + 1) * 6, ph = (My - my + 1) * 6;
        int x0 = HX + sway - pw / 2, y0 = tipY - ph + 2;
        for (auto& c : cs) ToyBlocks::cell6(f, x0 + (c.first - mx) * 6, y0 + (c.second - my) * 6, p);
    }
    f.disc(HX - 11 + sway, tipY - 1, 2, C(TENT)); f.disc(HX + 11 + sway, tipY - 1, 2, C(TENT));
}
void BeachRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    class Item { public: float y; int order, kind, k; };
    vector<Item> items;
    for (int k = 0; k < 5; ++k) if (build(k) > 0) items.push_back({ static_cast<float>(SPOTS[k].y), 0, 0, k });
    items.push_back({ ky_ + 0.5f, 1, 1, 0 });
    items.push_back({ static_cast<float>(BKT_Y + 52), 0, 2, 0 });
    items.push_back({ static_cast<float>(OCTO_Y + 54), 0, 3, 0 });
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y != b.y ? a.y < b.y : a.order < b.order; });
    int goo0 = goneT_ >= 0 ? goneT_ + 4 : -1;
    for (auto& it : items) {
        if (it.kind == 0) {
            drawCastle(f, it.k, build(it.k), t);
            if (goo0 >= 0 && t >= goo0 + it.k * 2) drawCastleGoo(f, it.k, t - goo0 - it.k * 2);
        } else if (it.kind == 1) drawKid(f, t);
        else if (it.kind == 2) drawBucket(f, t);
        else drawOcto(f, t);
    }
    if (puffT_ == t)
        for (int j = 0; j < 5; ++j) {
            float a = PI + (j + 0.5f) * PI / 5;
            f.set(puffX_ + rnd(cos(a) * 7), puffY_ + rnd(sin(a) * 3), C(252, 232, 196));
        }
}
void BeachRound::drawShovelBucket(Frame& f, int x, int y) {
    uint8_t ol = C(OUT), blue = C(60, 130, 210), blueD = C(40, 96, 170), blueL = C(120, 180, 240);
    uint8_t red = C(214, 58, 62), redD = C(150, 34, 44), wood = C(196, 146, 94);
    f.poly({ {x - 9, y - 6}, {x + 3, y - 6}, {x + 2, y + 7}, {x - 8, y + 7} }, ol);
    f.poly({ {x - 8, y - 5}, {x + 2, y - 5}, {x + 1, y + 6}, {x - 7, y + 6} }, blue);
    f.vline(x, y - 5, y + 6, blueD); f.vline(x - 7, y - 4, y + 5, blueL);
    f.hline(x - 9, x + 3, y - 6, ol); f.hline(x - 8, x + 2, y - 5, blueL);
    for (int k = 0; k < 9; ++k) f.set(x - 3 + rnd(cos(PI + k * PI / 8) * 6), y - 6 + rnd(sin(PI + k * PI / 8) * 5), ol);
    f.line(x + 4, y - 9, x + 9, y + 2, ol); f.line(x + 5, y - 9, x + 10, y + 2, wood);
    f.poly({ {x + 7, y + 1}, {x + 13, y - 1}, {x + 15, y + 6}, {x + 10, y + 8} }, ol);
    f.poly({ {x + 8, y + 2}, {x + 12, y + 0}, {x + 14, y + 6}, {x + 10, y + 7} }, red);
    f.line(x + 11, y + 1, x + 13, y + 6, redD);
}
void BeachRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    if (endT_ >= 0 && t - endT_ < 26) {
        int m = t - endT_, sx0 = SPOTS[4].x, sy0 = SPOTS[4].y - 24;
        float u = clamp01((m - 4) / 10.0f);
        int ix = sx0 + rnd((34 - sx0) * u), iy = sy0 + rnd((236 - sy0) * u) - rnd(sin(u * PI) * 60);
        if (m < 12) for (int k = 0; k < 8; ++k) { float a = k * 0.785f + m * 0.3f; sparkle(f, sx0 + rnd(cos(a) * (18 + m)), sy0 + rnd(sin(a) * (10 + m / 2))); }
        if (m >= 14) drawMemoryBook(f, 6, 226 - ((m >= 14 && m < 18) ? (m % 2 ? 3 : 1) : 0));
        if (m < 15) drawShovelBucket(f, ix, iy);
        if (m >= 14 && m < 24) {
            int r = (m - 14) * 3 + 6;
            for (int k = 0; k < 8; ++k) { float a = k * 0.785f; sparkle(f, 34 + rnd(cos(a) * r), 242 + rnd(sin(a) * r * 0.7f)); }
            if (m < 20) Font::center(f, "SHOVEL & BUCKET!", 56, 212 - (m - 14), C(252, 228, 152), C(OUT));
        }
    }
    int call0 = endT_ >= 0 ? endT_ + 12 : -1;
    if (call0 >= 0 && t >= call0 && t < call0 + 22) {
        int bx = 8, by = 98;
        bool pop = t - call0 < 2;
        int w = pop ? 44 : 84, h = pop ? 12 : 20;
        uint8_t ol = C(OUT), paper = C(255, 255, 250);
        f.rect(bx, by, w + 1, h + 1, ol); f.rect(bx + 1, by + 1, w - 1, h - 1, paper);
        f.set(bx, by, paper); f.set(bx + w, by, paper); f.set(bx, by + h, paper); f.set(bx + w, by + h, paper);
        f.poly({ {bx + 1, by + h - 7}, {bx + 1, by + h - 2}, {0, by + h + 2} }, paper);
        f.line(0, by + h + 2, bx, by + h - 7, ol); f.line(0, by + h + 2, bx, by + h - 2, ol);
        if (!pop) { Font::draw(f, "HONEY!", bx + 4, by + 3, C(60, 40, 40)); Font::draw(f, "TIME FOR BED!", bx + 4, by + 11, C(190, 70, 70)); }
    }
    float dusk = min(0.35f, min(10, lines_) / 10.0f * 0.25f + (endT_ >= 0 ? 0.1f * ease((t - endT_) / 30.0f) : 0.0f));
    int goo0 = goneT_ >= 0 ? goneT_ + 4 : -1;
    if (goo0 >= 0 && t >= goo0) dusk = min(0.6f, dusk + 0.25f * ease((t - goo0) / 40.0f));
    float black = 0;
    if (t - startT_ < 8) black = 1 - (t - startT_) / 8.0f;
    if (goo0 >= 0 && t >= goo0 + 52) black = min(1.0f, (t - goo0 - 52 + 1) / 10.0f);
    for (auto& c : palette()) c = mix(mix(c, RGB{ 60, 50, 110 }, dusk), RGB{ 0, 0, 0 }, black);
}
void BeachRound::kneelPoint(int i, float& x, float& y) const { x = static_cast<float>(SPOTS[i].x - SHAPE[SPOTS[i].v][0] - 18); y = static_cast<float>(SPOTS[i].y); }
void BeachRound::patReach(int i, int& dx, int& h) const { const int* s = SHAPE[SPOTS[i].v]; dx = s[0] + 18 + s[2]; h = s[3]; }
float BeachRound::pathLen() const {
    float L = 0;
    for (size_t i = 0; i + 1 < path_.size(); ++i) L += hypot(path_[i + 1].first - path_[i].first, path_[i + 1].second - path_[i].second);
    return L;
}
void BeachRound::along(float s, float& x, float& y) const {
    for (size_t i = 0; i + 1 < path_.size(); ++i) {
        float dx = path_[i + 1].first - path_[i].first, dy = path_[i + 1].second - path_[i].second, L = hypot(dx, dy);
        if (s <= L) { x = path_[i].first + dx * s / L; y = path_[i].second + dy * s / L; return; }
        s -= L;
    }
    x = path_.back().first; y = path_.back().second;
}
void BeachRound::reset() {
    mode_ = Idle; mt_ = 0; cur_ = 0; pending_ = 0; path_.clear();
    for (int i = 0; i < 5; ++i) { halves_[i] = 0; part_[i] = 0; }
    kneelPoint(0, kx_, ky_); pose_ = P_KNEEL;
    puffT_ = -1; startT_ = lastT_; lines_ = 0; clearT_ = -100; clearN_ = 0; endT_ = -1; goneT_ = -1;
}
void BeachRound::specialEvent(int) { if (cur_ < 5 && endT_ < 0) ++pending_; }
void BeachRound::onLinesCleared(const vector<int>& rows, int totalLines, int t) {
    lines_ = min(10, totalLines);
    clearT_ = t; clearN_ = static_cast<int>(rows.size());
    pending_ += static_cast<int>(rows.size());
}
void BeachRound::update(const PlayStateReader& g, int t) {
    if (t < lastT_ || lastT_ == 0) startT_ = t;
    lastT_ = t;
    if (g.next() != piece_) { prevPiece_ = piece_; piece_ = g.next(); }
    bool blink = t % 37 == 0;
    if (mode_ == Idle && pending_ > 0 && cur_ < 5) { mode_ = Build; mt_ = 0; }
    switch (mode_) {
    case Idle:
        pose_ = P_KNEEL + (t % 8);
        break;
    case Build: {
        int ph = mt_ % PAT_LEN, done = mt_ / PAT_LEN + (ph == PAT_LEN - 1 ? 1 : 0);
        part_[cur_] = done;
        pose_ = P_BUILD + cur_ * 16 + halves_[cur_] * 8 + mt_;
        if (ph == PAT_LEN - 1) {
            int dx, h; patReach(cur_, dx, h);
            puffT_ = t; puffX_ = rnd(kx_) + dx; puffY_ = rnd(ky_) - rnd(h * build(cur_));
        }
        if (++mt_ == PATS * PAT_LEN) {
            ++halves_[cur_]; part_[cur_] = 0; --pending_;
            if (halves_[cur_] == 2) mode_ = Happy; else mode_ = pending_ > 0 ? Build : Idle;
            mt_ = 0;
        }
        break;
    }
    case Happy:
        pose_ = P_HAPPY + (blink ? 1 : 0);
        if (++mt_ == HAPPY) {
            mt_ = 0;
            if (cur_ == 4) { mode_ = End; endT_ = t + 1; }
            else {
                float x1, y1; kneelPoint(cur_ + 1, x1, y1);
                path_.clear(); path_.push_back({ kx_, ky_ });
                if (fabs(y1 - ky_) < 1 && x1 > kx_) {
                    float lane = ky_ + 13;
                    path_.push_back({ kx_ + 6, lane }); path_.push_back({ x1 - 6, lane });
                }
                path_.push_back({ x1, y1 });
                mode_ = Stand;
            }
        }
        break;
    case Stand:
        pose_ = P_STANDUP + mt_;
        if (++mt_ == STAND) { mode_ = Run; mt_ = 0; }
        break;
    case Run: {
        float x, y, nx, ny;
        along(mt_ * SPEED, x, y); along(mt_ * SPEED + 2, nx, ny);
        kx_ = static_cast<float>(rnd(x)); ky_ = static_cast<float>(rnd(y));
        pose_ = (nx >= x - 0.1f ? P_RUN_R : P_RUN_L) + t % 4;
        if (++mt_ * SPEED >= pathLen()) { kx_ = path_.back().first; ky_ = path_.back().second; mode_ = Kneel; mt_ = 0; }
        break;
    }
    case Kneel:
        pose_ = mt_ == 0 ? P_STANDUP : P_KNEEL;
        if (++mt_ == KNEEL) { ++cur_; mode_ = Idle; mt_ = 0; }
        break;
    case End: {
        int e = t - endT_;
        if (e < 0) { pose_ = P_HAPPY; break; }
        if (e < STAND) pose_ = P_STANDUP + e;
        else if (e < 12) pose_ = P_END;
        else if (e < 18) pose_ = P_END + 1;
        else if (e < 30) pose_ = P_WAVE + (e - 18) % 4;
        else if (goneT_ < 0) {
            if (e == 30) {
                path_.clear();
                path_.push_back({ kx_, ky_ });
                path_.push_back({ kx_ - 22, 200 });
                path_.push_back({ -40, 196 });
            }
            float x, y; along((e - 30) * SPEED, x, y);
            kx_ = static_cast<float>(rnd(x)); ky_ = static_cast<float>(rnd(y));
            pose_ = P_RUN_L + t % 4;
            if ((e - 30) * SPEED > pathLen()) goneT_ = t;
        }
        break;
    }
    }
}
