#pragma once
#include "ui/PixelArt.h"
#include <algorithm>
#include <cmath>
using namespace std;

namespace Stage3Art {
inline const RGB OLC{ 40, 30, 36 };
inline const Sprite HEAD_SIDE("stage 3 head, facing right", {
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
class Look { public: RGB hair, hairL, top, topD, bottom, bottomD, accent; bool tie, dress, longHair, glasses, old, child; };
inline const Look HERO_SUIT{ {168, 92, 44}, {206, 128, 64}, {44, 52, 86}, {32, 38, 64}, {44, 52, 86}, {32, 38, 64}, {190, 50, 60}, true, false, false, false, false, false };
inline const Look HERO_TUX{ {168, 92, 44}, {206, 128, 64}, {30, 30, 36}, {20, 20, 26}, {30, 30, 36}, {20, 20, 26}, {250, 250, 246}, true, false, false, false, false, false };
inline const Look HERO_HOME{ {168, 92, 44}, {206, 128, 64}, {120, 150, 110}, {90, 116, 84}, {70, 76, 96}, {52, 56, 74}, {250, 250, 246}, false, false, false, false, false, false };
inline const Look HERO_OLD{ {200, 200, 206}, {236, 236, 240}, {150, 120, 96}, {116, 90, 72}, {90, 92, 104}, {68, 70, 80}, {250, 250, 246}, false, false, false, true, true, false };
inline const Look BRIDE{ {120, 70, 46}, {160, 104, 70}, {252, 250, 246}, {222, 218, 214}, {252, 250, 246}, {222, 218, 214}, {250, 190, 210}, false, true, true, false, false, false };
inline const Look WIFE{ {120, 70, 46}, {160, 104, 70}, {214, 120, 140}, {176, 90, 110}, {80, 84, 110}, {60, 62, 84}, {250, 250, 246}, false, true, true, false, false, false };
inline const Look GIRL{ {120, 70, 46}, {160, 104, 70}, {250, 200, 90}, {214, 160, 60}, {230, 110, 130}, {190, 80, 100}, {250, 250, 246}, false, true, true, false, false, true };
inline const Look DAUGHTER{ {120, 70, 46}, {160, 104, 70}, {110, 150, 200}, {80, 116, 166}, {60, 64, 84}, {44, 48, 64}, {250, 250, 246}, false, false, true, false, false, false };
inline void recolor(Frame& tmp, int w, int h, uint8_t from, uint8_t to) { for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) if (tmp.get(i, j) == from) tmp.set(i, j, to); }
inline void outline2x(Frame& f, const Frame& tmp, int W, int H, int dx, int dy, bool flip, int S) {
    uint8_t key = C(255, 0, 255), ol = C(OLC);
    for (int j = -1; j <= H; ++j) for (int i = -1; i <= W; ++i) {
        int si = flip ? W - 1 - i : i;
        uint8_t c = (si >= 0 && si < W && j >= 0 && j < H) ? tmp.get(si, j) : key;
        bool edge = false;
        if (c == key) for (int k = 0; k < 4 && !edge; ++k) { int ni = si + (k == 0) - (k == 1), nj = j + (k == 2) - (k == 3); if (ni >= 0 && ni < W && nj >= 0 && nj < H && tmp.get(ni, nj) != key) edge = true; }
        if (c == key && !edge) continue;
        f.rect(dx + i * S, dy + j * S, S, S, c == key ? ol : c);
    }
}
enum Pose { STAND, SIT, WALK };
enum Arm { REST, TYPE, UP, HOLD, OUT };
enum Mood { CALM, SMILE, SLEEPY, TALK, SAD };
inline void person(Frame& f, int cx, int floor, const Look& lk, bool flip, Pose pose, Arm arm, Mood mood, int t, int S = 2, int* handX = nullptr, int* handY = nullptr) {
    static Frame tmp;
    const int W = 30, H = 40;
    uint8_t key = C(255, 0, 255), ol = C(OLC), top = C(lk.top), topD = C(lk.topD), bot = C(lk.bottom), botD = C(lk.bottomD), skin = C(246, 204, 164), shoe = C(30, 26, 30), shoeL = C(80, 76, 84), shirt = C(246, 246, 242);
    tmp.rect(0, 0, W, H, key);
    int fy = H - 1, legLen = lk.child ? 7 : 12, torso = lk.child ? 8 : 11;
    int hip = fy - legLen - 1, sh = hip - torso, oy = sh - 11 + (lk.old ? 1 : 0), ox = 6 + (lk.old ? 1 : 0);
    if (pose == SIT) { hip = fy - 9; sh = hip - torso; oy = sh - 11 + (lk.old ? 1 : 0); }
    int bx = 8;
    if (lk.longHair) { tmp.rect(ox + 1, oy + 2, 6, 12, C(lk.hair)); tmp.vline(ox + 1, oy + 3, oy + 13, C(lk.hairL)); }
    if (pose == SIT) {
        tmp.rect(bx, hip - 1, 10, 4, lk.dress ? top : bot); tmp.rect(bx + 8, hip + 1, 3, fy - hip - 2, lk.dress ? skin : bot);
        tmp.rect(bx + 7, fy - 1, 6, 2, shoe); tmp.hline(bx + 8, bx + 12, fy - 1, shoeL);
        if (lk.dress) tmp.poly({ {bx - 1, hip - 2}, {bx + 11, hip - 2}, {bx + 12, hip + 4}, {bx - 1, hip + 4} }, top);
    } else if (lk.dress && !lk.child) {
        int sw = pose == WALK ? ((t / 2) % 2 ? 1 : -1) : 0;
        tmp.poly({ {bx + 1, hip - 3}, {bx + 8, hip - 3}, {bx + 12 + sw, fy - 1}, {bx - 3 + sw, fy - 1} }, top);
        tmp.vline(bx + 4, hip, fy - 2, topD); tmp.vline(bx + 8, hip + 3, fy - 2, topD); tmp.hline(bx - 3 + sw, bx + 12 + sw, fy - 1, topD);
        tmp.rect(bx + 6, fy, 4, 1, shoe);
    } else {
        int st = pose == WALK ? (t / 2) % 4 : 0, a = st == 1 ? 3 : st == 3 ? -3 : 0;
        auto leg = [&](int fx, uint8_t c) { tmp.taper(bx + 4, hip, fx, fy - 2, 3, 3, c); tmp.rect(fx - 1, fy - 1, 5, 2, shoe); tmp.set(fx + 3, fy - 1, shoeL); };
        uint8_t legC = lk.child ? skin : bot, legD = lk.child ? C(214, 164, 128) : botD;
        leg(bx + 4 - a, legD); leg(bx + 5 + a, legC);
        if (lk.child) tmp.poly({ {bx, hip - 3}, {bx + 9, hip - 3}, {bx + 11, hip + 2}, {bx - 2, hip + 2} }, bot);
    }
    tmp.rect(bx, sh, 9, hip - sh, top); tmp.vline(bx, sh, hip - 1, topD);
    if (lk.tie) { tmp.poly({ {bx + 6, sh}, {bx + 9, sh}, {bx + 8, sh + 5} }, shirt); tmp.vline(bx + 8, sh + 1, sh + 6, C(lk.accent)); tmp.set(bx + 7, sh + 1, C(lk.accent)); }
    if (lk.dress && !lk.child) { tmp.hline(bx, bx + 8, sh, C(lk.accent)); tmp.set(bx + 7, sh + 3, C(lk.accent)); }
    if (lk.old) { tmp.rect(bx + 1, sh, 7, 2, topD); tmp.vline(bx + 4, sh + 2, hip - 1, topD); for (int k = 0; k < 3; ++k) tmp.set(bx + 5, sh + 3 + k * 3, C(lk.accent)); }
    blit(tmp, HEAD_SIDE, ox, oy);
    recolor(tmp, W, H, C(168, 92, 44), C(lk.hair)); recolor(tmp, W, H, C(206, 128, 64), C(lk.hairL));
    if (lk.longHair) { tmp.rect(ox + 2, oy, 7, 3, C(lk.hair)); tmp.vline(ox + 2, oy + 3, oy + 9, C(lk.hair)); }
    if (lk.glasses) { tmp.hline(ox + 7, ox + 13, oy + 5, ol); tmp.rect(ox + 10, oy + 4, 3, 3, ol); tmp.set(ox + 11, oy + 5, C(200, 220, 240)); }
    if (lk.old) { tmp.set(ox + 12, oy + 3, C(160, 150, 150)); tmp.hline(ox + 9, ox + 10, oy + 7, C(214, 164, 128)); }
    if (mood == SMILE) { tmp.set(ox + 11, oy + 5, skin); tmp.hline(ox + 10, ox + 12, oy + 5, ol); tmp.set(ox + 11, oy + 4, ol); tmp.hline(ox + 10, ox + 12, oy + 8, C(176, 80, 84)); tmp.set(ox + 13, oy + 7, C(176, 80, 84)); tmp.set(ox + 9, oy + 7, C(240, 160, 150)); }
    if (mood == SLEEPY) { tmp.set(ox + 11, oy + 5, skin); tmp.hline(ox + 10, ox + 12, oy + 6, ol); }
    if (mood == TALK && (t / 2) % 2) { tmp.set(ox + 11, oy + 8, ol); tmp.set(ox + 12, oy + 9, ol); }
    if (mood == SAD) { tmp.set(ox + 10, oy + 4, ol); tmp.hline(ox + 10, ox + 12, oy + 9, C(176, 80, 84)); tmp.set(ox + 11, oy + 8, skin); }
    int sx = bx + 4, sy = sh + 2, ax = bx + 6, ay = hip + 1;
    if (arm == TYPE) { ax = bx + 15 + ((t % 2) ? 1 : 0); ay = hip - 2; }
    if (arm == UP) { ax = bx + 9; ay = oy - 2 + (t % 2); }
    if (arm == HOLD) { ax = bx + 12; ay = sh + 6; }
    if (arm == OUT) { ax = bx + 17; ay = sh + 4; }
    if (pose == WALK && arm == REST) ax += ((t / 2) % 4 == 1 ? 3 : (t / 2) % 4 == 3 ? -3 : 0);
    tmp.taper(sx, sy, ax, ay, 3, 3, topD); tmp.rect(ax - 1, ay - 1, 3, 2, skin);
    int dx = flip ? cx - (W - 1 - (bx + 4)) * S : cx - (bx + 4) * S, dy = floor - fy * S;
    f.ellipse(cx, floor + 1, 9 * S / 2 + 4, 2 + S / 2, C(OLC));
    if (handX) *handX = flip ? dx + (W - 1 - ax) * S + S / 2 : dx + ax * S + S / 2;
    if (handY) *handY = dy + ay * S + S / 2;
    outline2x(f, tmp, W, H, dx, dy, flip, S);
}
inline void chair(Frame& f, int cx, int floor, bool flip, bool rocking, int t) {
    uint8_t ol = C(OLC), wood = C(170, 120, 80), woodD = C(130, 90, 60), woodL = C(200, 150, 100);
    int d = flip ? -1 : 1, rock = rocking ? static_cast<int>(lround(sin(t * 0.3f) * 2)) : 0;
    int back = cx - 13 * d, front = cx + 8 * d, seat = floor - 17;
    auto vbar = [&](int x, int y0, int y1) { f.rect(x - 2, y0, 5, y1 - y0, ol); f.rect(x - 1, y0, 3, y1 - y0, wood); f.vline(x - 1, y0, y1 - 1, woodL); };
    vbar(back, seat - 34 + rock, floor - (rocking ? 4 : 0));
    vbar(front, seat + 2, floor - (rocking ? 4 : 0));
    int x0 = min(back, front) - 2, x1 = max(back, front) + 2;
    f.rect(x0, seat - 1, x1 - x0 + 1, 5, ol); f.rect(x0 + 1, seat, x1 - x0 - 1, 3, wood); f.hline(x0 + 1, x1 - 1, seat, woodL);
    f.rect(min(back, back + 6 * d) - 1, seat - 22 + rock, 8, 3, ol); f.hline(min(back, back + 6 * d), max(back, back + 6 * d), seat - 21 + rock, woodD);
    if (rocking) for (int x = x0 - 6; x <= x1 + 6; ++x) { float u = (x - (x0 + x1) / 2.0f) / ((x1 - x0) / 2.0f + 6); int y = floor - 3 + static_cast<int>(lround(u * u * 3)) + static_cast<int>(lround(u * rock)); f.set(x, y, ol); f.set(x, y - 1, wood); }
}
inline void shade(Frame& f, int x0, int y0, int x1, int y1, int level) {
    if (level <= 0) return;
    static const RGB lo[3] = { {70, 52, 84}, {34, 34, 70}, {12, 16, 40} }, hi[3] = { {246, 206, 196}, {184, 180, 222}, {118, 138, 196} };
    static const RGB keep[6] = { {250, 214, 120}, {255, 236, 160}, {170, 214, 252}, {244, 240, 214}, {230, 230, 250}, {255, 248, 220} };
    int l = min(3, level) - 1;
    int map[256];
    for (int& m : map) m = -1;
    for (int y = max(0, y0); y < min(Frame::H, y1); ++y) for (int x = max(0, x0); x < min(Frame::W, x1); ++x) {
        uint8_t c = f.get(x, y);
        if (map[c] < 0) {
            RGB v = palette()[c];
            bool lit = false;
            for (auto& k : keep) if (k.r == v.r && k.g == v.g && k.b == v.b) lit = true;
            float lum = (v.r * 0.3f + v.g * 0.59f + v.b * 0.11f) / 255.0f;
            map[c] = lit ? c : C(mix(lo[l], hi[l], min(7, static_cast<int>(lum * 8)) / 7.0f));
        }
        f.set(x, y, static_cast<uint8_t>(map[c]));
    }
}
inline void gray(Frame& f, int x0, int y0, int x1, int y1, float k) {
    if (k <= 0) return;
    int map[256];
    for (int& m : map) m = -1;
    for (int y = max(0, y0); y < min(Frame::H, y1); ++y) for (int x = max(0, x0); x < min(Frame::W, x1); ++x) {
        uint8_t c = f.get(x, y);
        if (map[c] < 0) {
            RGB v = palette()[c];
            int lum = static_cast<int>(v.r * 0.3f + v.g * 0.59f + v.b * 0.11f);
            lum = lum / 24 * 24 + 12;
            map[c] = C(mix(v, RGB{ lum, lum, lum }, min(1.0f, k)));
        }
        f.set(x, y, static_cast<uint8_t>(map[c]));
    }
}
inline void armchairBack(Frame& f, int cx, int floor, RGB c) {
    uint8_t ol = C(OLC), base = C(c), dark = C(mix(c, RGB{ 40, 20, 24 }, 0.16f)), lite = C(mix(c, RGB{ 255, 246, 220 }, 0.3f));
    f.ellipse(cx - 8, floor + 1, 26, 3, C(mix(c, RGB{ 40, 20, 24 }, 0.55f)));
    f.rect(cx - 27, floor - 56, 19, 48, ol); f.disc(cx - 20, floor - 56, 7, ol); f.disc(cx - 15, floor - 56, 7, ol);
    f.rect(cx - 26, floor - 56, 17, 47, base); f.disc(cx - 20, floor - 56, 6, base); f.disc(cx - 15, floor - 56, 6, base);
    f.vline(cx - 25, floor - 56, floor - 10, lite); f.hline(cx - 22, cx - 14, floor - 61, lite); f.vline(cx - 10, floor - 54, floor - 10, dark);
    f.rect(cx - 13, floor - 50, 3, 30, dark);
    f.rect(cx - 25, floor - 18, 34, 10, ol); f.rect(cx - 24, floor - 17, 32, 8, base); f.disc(cx + 6, floor - 13, 4, ol); f.disc(cx + 6, floor - 13, 3, base); f.hline(cx - 24, cx + 6, floor - 17, lite);
}
inline void armchairFront(Frame& f, int cx, int floor, RGB c) {
    uint8_t ol = C(OLC), base = C(c), dark = C(mix(c, RGB{ 40, 20, 24 }, 0.16f)), lite = C(mix(c, RGB{ 255, 246, 220 }, 0.3f));
    f.rect(cx - 24, floor - 26, 27, 22, ol); f.rect(cx - 23, floor - 25, 25, 20, dark); f.rect(cx - 21, floor - 23, 21, 16, base); f.hline(cx - 21, cx - 1, floor - 23, lite);
    f.rect(cx - 27, floor - 32, 33, 8, ol); f.disc(cx + 5, floor - 28, 4, ol); f.rect(cx - 26, floor - 31, 31, 6, base); f.disc(cx + 5, floor - 28, 3, base); f.hline(cx - 26, cx + 4, floor - 31, lite); f.hline(cx - 26, cx + 4, floor - 25, dark);
    f.disc(cx + 5, floor - 28, 1, dark);
    f.rect(cx - 27, floor - 6, 36, 5, ol); f.rect(cx - 26, floor - 5, 34, 3, dark); f.hline(cx - 26, cx + 7, floor - 5, base);
    f.rect(cx - 25, floor - 1, 3, 2, ol); f.rect(cx + 4, floor - 1, 3, 2, ol);
}
inline void star(Frame& f, int x, int y) { f.set(x, y, C(255, 250, 200)); f.set(x - 1, y, C(250, 222, 110)); f.set(x + 1, y, C(250, 222, 110)); f.set(x, y - 1, C(250, 222, 110)); f.set(x, y + 1, C(250, 222, 110)); }
}
