#include "ui/LakeRound.h"
#include "ui/PixelFont.h"
#include <cmath>
#include <cstdlib>
namespace {
const RGB OUT{ 42, 26, 28 };
const RGB GRASS{ 118, 176, 82 }, GRASS_L{ 160, 204, 94 }, GRASS_D{ 80, 140, 74 }, HAZE{ 156, 198, 132 };
const RGB BARK{ 126, 80, 58 }, BARK_L{ 164, 110, 74 }, BARK_D{ 88, 54, 42 };
int wave(int t, float period, float amp, float phase = 0) {
    return static_cast<int>(std::lround(std::sin(t * 6.2832f / period + phase) * amp));
}
void tuft(Frame& f, int x, int y) {
    uint8_t d = C(GRASS_D), l = C(GRASS_L);
    f.set(x, y, d); f.set(x - 1, y - 1, d); f.set(x + 1, y - 1, d); f.set(x, y - 1, d); f.set(x, y - 2, l);
}
void flower(Frame& f, int x, int y, RGB petal) {
    f.vline(x, y, y + 3, C(70, 130, 70));
    f.set(x, y - 1, C(petal)); f.set(x - 1, y, C(petal)); f.set(x + 1, y, C(petal)); f.set(x, y, C(250, 230, 120));
}
void cloud(Frame& f, int x, int y, int s) {
    uint8_t w = C(252, 252, 255), u = C(214, 230, 246), o = C(190, 214, 238);
    f.ellipse(x, y + 4, 22 * s / 10, 4, o);
    f.ellipse(x - 10 * s / 10, y + 1, 9 * s / 10, 6, w);
    f.ellipse(x + 2, y - 3, 11 * s / 10, 9, w);
    f.ellipse(x + 13 * s / 10, y + 1, 9 * s / 10, 6, w);
    f.hline(x - 18 * s / 10, x + 20 * s / 10, y + 6, u);
}
void clump(Frame& f, int cx, int cy, int r, int sway) {
    uint8_t ol = C(36, 84, 52), d = C(56, 120, 60), m = C(86, 158, 72), l = C(132, 194, 92), h = C(176, 220, 120);
    int bumps[7][3] = { {0,0,10}, {-7,3,7}, {7,3,7}, {-4,-6,7}, {5,-5,7}, {-9,-2,5}, {9,-1,5} };
    for (auto& b : bumps) f.ellipse(cx + b[0] * r / 10 + sway, cy + b[1] * r / 10, b[2] * r / 10 + 1, b[2] * r / 10 + 1, ol);
    for (auto& b : bumps) f.ellipse(cx + b[0] * r / 10 + sway, cy + b[1] * r / 10, b[2] * r / 10, b[2] * r / 10, d);
    for (auto& b : bumps) f.ellipse(cx + b[0] * r / 10 + sway - 1, cy + b[1] * r / 10 - 1, b[2] * r / 10 - 1, b[2] * r / 10 - 2, m);
    for (auto& b : bumps) f.ellipse(cx + b[0] * r / 10 + sway - 2, cy + b[1] * r / 10 - 2, b[2] * r / 20, b[2] * r / 20, l);
    f.set(cx - r / 2 + sway, cy - r / 2, h); f.set(cx + sway, cy - r / 2 - 1, h);
    for (int k = 0; k < r; ++k) {
        int lx = cx + sway + ((k * 13) % (2 * r)) - r, ly = cy + ((k * 7) % r) - r / 2;
        if (f.get(lx, ly) == m) { f.set(lx, ly, l); f.set(lx + 1, ly, l); f.set(lx, ly + 1, d); }
    }
    uint8_t pink = C(244, 170, 186);
    for (int k = 0; k < 4; ++k) f.set(cx + ((k * 7) % (r + 1)) - r / 2 + sway, cy + ((k * 5) % (r / 2 + 1)) - r / 4, pink);
}
}
void LakeRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    RGB sky[6] = { {110,164,218}, {126,178,226}, {144,192,234}, {164,206,240}, {186,220,244}, {204,230,246} };
    for (int y = 0; y < 60; ++y) {
        int b = std::min(5, y / 10);
        for (int x = 0; x < 480; ++x) {
            int bb = b;
            if (y % 10 == 9 && b < 5 && (x & 1)) bb = b + 1;
            f.set(x, y, C(sky[bb]));
        }
    }
    f.disc(150, -2, 24, C(186, 220, 244)); f.disc(150, -2, 20, C(214, 234, 248));
    f.disc(150, -2, 15, C(240, 246, 236)); f.disc(150, -4, 11, C(252, 250, 224));
    int drift = (static_cast<int>(cloudX_)) % 560;
    cloud(f, (175 + drift) % 560 - 40, 14, 12);
    cloud(f, (290 + drift) % 560 - 40, 12, 14);
    cloud(f, (40 + drift) % 560 - 40, 30, 8);
    uint8_t hill = C(128, 168, 160), trees = C(104, 146, 128), treesD = C(88, 128, 116);
    for (int x = 0; x < 480; ++x) {
        int top = 54 - static_cast<int>(3 * std::sin(x * 0.03) + 2 * std::sin(x * 0.11));
        f.vline(x, top, 62, hill);
    }
    for (int x = 296; x < 480; ++x) {
        int h = 4 + ((x * 7) % 5) + ((x % 9 == 0) ? 4 : 0);
        f.vline(x, 56 - h, 60, (x % 6 < 3) ? trees : treesD);
    }
    for (int y = 62; y < 270; ++y)
        for (int x = 0; x < 480; ++x)
            f.set(x, y, (y < 82 && ((x + y) & 1) && (y < 67 || (y & 1))) ? C(HAZE) : C(GRASS));
    f.ditherEllipse(52, 103, 42, 8, C(GRASS_L)); f.ditherEllipse(222, 125, 28, 7, C(GRASS_L));
    f.ditherEllipse(278, 96, 25, 5, C(GRASS_L)); f.ditherEllipse(150, 56 + 30, 30, 4, C(GRASS_L));
    f.ditherEllipse(64, 68, 24, 7, C(GRASS_D)); f.ditherEllipse(46, 147, 26, 8, C(GRASS_D));
    f.ditherEllipse(56, 200, 22, 7, C(GRASS_D)); f.ditherEllipse(222, 182, 13, 4, C(GRASS_D));
    f.ellipse(152, 266, 98, 50, C(100, 160, 74));
    f.ellipse(148, 264, 94, 47, C(140, 192, 88));
    f.ditherEllipse(132, 252, 70, 30, C(160, 204, 94));
    for (int k = 0; k < 90; ++k) {
        int x = (k * 53 + 11) % 470 + 5, y = 70 + (k * 37) % 195;
        if (x > 298 && y < 236) continue;
        tuft(f, x, y);
    }
    flower(f, 18, 210, { 250,250,250 }); flower(f, 28, 213, { 244,170,90 }); flower(f, 296, 88, { 244,150,90 });
    flower(f, 304, 86, { 244,150,90 }); flower(f, 200, 108, { 236,200,240 }); flower(f, 365, 243, { 240,220,120 });
    flower(f, 372, 246, { 244,150,90 }); flower(f, 415, 256, { 244,150,90 });
    drawLake(f, t);
    drawCabin(f, t);
    drawTree(f, t);
    drawStump(f);
    uint8_t st = C(150, 150, 160), stL = C(196, 196, 204), stD = C(104, 104, 118);
    int stones[8][2] = { {214,212}, {228,220}, {300,215}, {330,213}, {342,220}, {444,212}, {472,183}, {442,236} };
    for (auto& s : stones) { f.ellipse(s[0], s[1], 4, 2, stD); f.ellipse(s[0], s[1] - 1, 3, 1, st); f.set(s[0] - 2, s[1] - 2, stL); }
    drawCommonUI(f);
}
void LakeRound::drawLake(Frame& f, int t) {
    Frame::Pts sand = { {302,62}, {442,62}, {444,120}, {472,132}, {476,200}, {464,232}, {380,234}, {334,220}, {316,206}, {302,172}, {298,120} };
    f.poly(sand, C(222, 204, 156));
    Frame::Pts wet = { {308,64}, {436,64}, {434,110}, {431,143}, {437,173}, {427,209}, {384,226}, {352,220}, {331,203}, {317,168}, {309,127} };
    f.poly(wet, C(196, 176, 136));
    Frame::Pts water = { {311,67}, {433,67}, {430,110}, {427,143}, {433,173}, {423,207}, {383,223}, {353,217}, {333,200}, {320,167}, {313,127} };
    f.poly(water, C(108, 166, 212));
    Frame::Pts mid = { {318,92}, {428,92}, {424,143}, {429,173}, {420,203}, {384,218}, {356,212}, {337,197}, {325,165}, {318,127} };
    f.poly(mid, C(98, 154, 206));
    Frame::Pts deep = { {326,128}, {422,128}, {425,173}, {416,198}, {385,213}, {358,207}, {341,192}, {330,163} };
    f.poly(deep, C(86, 142, 196));
    uint8_t foam = C(184, 222, 244);
    for (size_t i = 0; i < water.size(); ++i) {
        auto a = water[i], b = water[(i + 1) % water.size()];
        if (a.second == 67 && b.second == 67) continue;
        f.line(a.first, a.second, b.first, b.second, foam);
    }
    for (int k = 0; k < 26; ++k) {
        int y = 74 + (k * 23) % 140, x = 322 + (k * 41 + t / 3) % 98;
        if (y > 200 && (x < 345 || x > 410)) continue;
        f.hline(x, x + 3 + k % 3, y, foam);
    }
    uint8_t pad = C(86, 160, 96), padD = C(60, 120, 80);
    int pads[4][2] = { {340,193}, {372,210}, {390,213}, {350,206} };
    for (auto& p : pads) { f.ellipse(p[0], p[1], 5, 2, padD); f.ellipse(p[0], p[1] - 1, 4, 1, pad); }
    f.set(389, 211, C(250, 200, 220));
    for (int k = 0; k < 7; ++k) {
        int x = 311 + k * 3 + (k % 2), y0 = 115 + k * 12;
        f.vline(x, y0, y0 + 10, C(60, 110, 70));
        f.vline(x, y0 - 2, y0, C(130, 90, 60));
    }
}
void LakeRound::drawCabin(Frame& f, int t) {
    uint8_t ol = C(OUT), wood = C(156, 104, 62), woodD = C(114, 72, 50), logs = C(140, 86, 56), logsD = C(98, 60, 40);
    f.rect(387, 122, 40, 5, ol); f.rect(388, 123, 38, 3, wood); f.hline(388, 425, 123, C(194, 142, 88));
    for (int k = 0; k < 4; ++k) f.rect(392 + k * 10, 127, 2, 9, woodD);
    for (int k = 0; k < 5; ++k) f.rect(428 + k * 12, 118, 3, 18, logsD);
    f.rect(423, 113, 57, 6, ol); f.rect(424, 114, 55, 4, wood); f.hline(424, 478, 114, C(194, 142, 88));
    f.rect(431, 78, 45, 36, ol);
    f.rect(432, 79, 43, 34, logs);
    for (int y = 82; y < 113; y += 5) f.hline(432, 474, y, logsD);
    f.rect(449, 87, 15, 14, ol); f.rect(450, 88, 13, 12, C(252, 222, 110));
    f.vline(456, 88, 99, ol); f.hline(450, 462, 94, ol);
    f.rect(437, 95, 9, 18, ol); f.rect(438, 96, 7, 17, C(84, 48, 40)); f.set(443, 104, C(220, 190, 110));
    for (int k = 0; k < 19; ++k) f.hline(428 + k * 3 / 2 - 4, 479 - k * 3 / 2 + 4, 79 - k, k < 1 ? ol : C(176, 74, 62));
    f.line(424, 79, 451, 60, ol);
    f.line(451, 60, 480, 79, ol);
    for (int k = 2; k < 18; k += 4) f.hline(432 + k, 470 - k, 79 - k, C(146, 56, 50));
    f.line(427, 78, 450, 62, C(214, 112, 90));
    f.rect(463, 61, 7, 12, ol); f.rect(464, 62, 5, 11, C(150, 150, 160));
    float w = wind(t);
    for (int k = 0; k < 3; ++k) {
        int age = (t + k * 8) % 24, sx = 466 + age / 6 + static_cast<int>(w * age * 2), sy = 58 - static_cast<int>(age * (1 - 0.8f * w));
        f.disc(sx, sy, 1 + age / 10, C(236, 240, 244));
    }
    int c = endStart_ >= 0 ? endFrame_ : -1;
    bool open = c >= 3 && c < 118;
    if (open) {
        int k = std::min(3, c - 2);
        f.rect(438, 96, 7, 17, C(120, 92, 60)); f.rect(439, 98, 5, 15, C(250, 206, 120));
        f.rect(438, 96, 7 - k * 2, 17, C(84, 48, 40));
        if (c >= 5 && c < 102) {
            int mx = 441, my = 113, wv = (c / 3) % 2;
            f.rect(mx - 2, my - 9, 5, 9, C(OUT)); f.rect(mx - 1, my - 8, 3, 8, C(226, 130, 140));
            f.disc(mx, my - 11, 2, C(246, 204, 164)); f.hline(mx - 2, mx + 2, my - 13, C(70, 44, 36)); f.set(mx - 3, my - 12, C(70, 44, 36));
            f.line(mx + 2, my - 7, mx + 4 + wv, my - 12 - wv, C(246, 204, 164));
        }
    }
    f.hline(426, 431, 95, ol); f.line(431, 99, 428, 95, ol);
    f.vline(427, 96, 97, ol);
    f.rect(425, 98, 5, 6, ol); f.rect(426, 99, 3, 4, C(255, 220, 120)); f.hline(425, 429, 97, ol);
    f.rect(451, 118, 15, 112, ol);
    f.rect(452, 118, 13, 112, wood);
    for (int y = 120; y < 230; y += 4) f.hline(452, 464, y, woodD);
    f.vline(451, 118, 229, ol); f.vline(465, 118, 229, ol);
    for (int y = 140; y < 230; y += 30) { f.rect(449, y, 3, 4, ol); f.rect(465, y, 3, 4, ol); }
    for (int y = 230; y < 270; ++y) {
        int cx = 458 - (y - 230) * 5 / 8, w = 6 + (y - 230) / 8;
        f.hline(cx - w - 1, cx + w + 1, y, C(150, 112, 76));
        f.hline(cx - w, cx + w, y, C(186, 146, 102));
        if (y % 7 == 0) f.set(cx - w / 2 + y % 5, y, C(208, 176, 132));
    }
}
void LakeRound::drawTree(Frame& f, int t) {
    float w = wind(t);
    uint8_t ol = C(OUT), b = C(BARK), bl = C(BARK_L), bd = C(BARK_D);
    struct Limb { int x0, y0, x1, y1; float w0, w1; };
    const Limb limbs[] = {
        {100,92, 66,52, 9,4}, {80,72, 56,72, 5,2}, {100,140, 56,140, 9,4}, {74,140, 52,155, 5,2},
        {100,190, 66,193, 8,3}, {180,100, 208,78, 7,3}, {180,165, 210,172, 7,3},
        {104,222, 98,250, 7,3}, {124,222, 132,246, 6,2}, {152,222, 148,244, 6,2}, {172,222, 184,250, 7,3},
        {182,212, 214,236, 7,2}, {98,212, 66,236, 7,2} };
    for (auto& l : limbs) f.taper(l.x0, l.y0, l.x1, l.y1, l.w0 + 2, l.w1 + 2, ol);
    for (auto& l : limbs) f.taper(l.x0, l.y0, l.x1, l.y1, l.w0, l.w1, b);
    for (auto& l : limbs) f.taper(l.x0, l.y0 - 2, l.x1, l.y1 - 1, l.w0 / 3, 1, bl);
    for (auto& l : limbs) f.taper(l.x0, l.y0 + 2, l.x1, l.y1 + 1, l.w0 / 4, 1, bd);
    clump(f, 58, 50, 24, wave(t, 26, 1.3f) + static_cast<int>(w * (3.2f + 1.4f * std::sin(1.9f * t))));
    clump(f, 42, 124, 26, wave(t, 26, 1.3f, 1.7f) + static_cast<int>(w * (3.2f + 1.4f * std::sin(1.9f * t + 1))));
    clump(f, 52, 184, 18, wave(t, 26, 1.3f, 3.4f) + static_cast<int>(w * (3.2f + 1.4f * std::sin(1.9f * t + 2))));
    clump(f, 212, 70, 14, wave(t, 26, 1.3f, 5.1f) + static_cast<int>(w * (3.2f + 1.4f * std::sin(1.9f * t + 3))));
    clump(f, 214, 164, 12, wave(t, 26, 1.3f, 6.8f) + static_cast<int>(w * (3.2f + 1.4f * std::sin(1.9f * t + 4))));
}
void LakeRound::drawStump(Frame& f) {
    uint8_t ol = C(OUT), side = C(182, 128, 80), sideD = C(140, 92, 58), sideL = C(206, 156, 100);
    uint8_t top = C(204, 160, 108), ring = C(176, 128, 82), topL = C(226, 188, 136);
    f.ellipse(276, 208, 46, 6, C(GRASS_D));
    Frame::Pts roots = { {228,208}, {248,196}, {252,170}, {300,170}, {304,196}, {326,204}, {300,206}, {290,214}, {276,206}, {262,214}, {254,206} };
    f.poly(roots, ol);
    Frame::Pts in = { {232,207}, {250,195}, {253,170}, {299,170}, {302,195}, {321,203}, {299,204}, {290,211}, {276,204}, {262,211}, {255,204} };
    f.poly(in, side);
    for (int x = 256; x < 300; x += 5) f.vline(x, 172 + (x % 3), 200 - (x % 7), sideD);
    f.vline(254, 172, 196, sideL); f.vline(257, 174, 192, sideL);
    f.ellipse(276, 168, 26, 9, ol);
    f.ellipse(276, 168, 25, 8, top);
    f.ellipse(276, 168, 18, 5, ring); f.ellipse(276, 168, 16, 4, top);
    f.ellipse(276, 168, 9, 2, ring); f.ellipse(276, 168, 7, 1, top);
    f.ellipse(268, 165, 6, 1, topL);
    f.line(266, 162, 284, 172, sideD);
    int mush[3][2] = { {237,200}, {312,198}, {320,201} };
    for (auto& m : mush) {
        f.vline(m[0], m[1], m[1] + 3, C(240, 230, 210));
        f.ellipse(m[0], m[1], 3, 2, ol); f.ellipse(m[0], m[1] - 1, 2, 1, C(214, 60, 52)); f.set(m[0] - 1, m[1] - 1, C(250, 240, 230));
    }
}
void LakeRound::drawBoat(Frame& f, int t) {
    int c = endStart_ >= 0 ? endFrame_ : -1;
    int bx = static_cast<int>(boatX_), by = static_cast<int>(boatY_);
    uint8_t ol = C(OUT), wood = C(160, 108, 66), woodL = C(196, 146, 94), woodD = C(116, 74, 48);
    uint8_t foam = C(184, 222, 244);
    bool rowing = c >= 14 && c < 46, kidIn = c < 46;
    f.hline(bx + 2, bx + 12, by + 21, foam); f.hline(bx + 24, bx + 36, by + 21, foam); f.hline(bx + 50, bx + 60, by + 21, foam);
    if (rowing && (c / 2) % 2) { f.hline(bx - 6, bx, by + 20, foam); f.hline(bx + 74, bx + 80, by + 18, foam); }
    if (kidIn) {
        int kx = bx + 37, ky = by - 11;
        f.ellipse(kx, ky + 3, 6, 6, ol); f.ellipse(kx, ky + 3, 5, 5, C(110, 70, 44));
        f.rect(kx - 4, ky + 4, 9, 6, C(246, 204, 164)); f.hline(kx - 4, kx + 4, ky + 3, C(110, 70, 44));
        f.set(kx - 2, ky + 6, C(34, 26, 34)); f.set(kx + 2, ky + 6, C(34, 26, 34)); f.set(kx, ky + 8, C(214, 120, 110));
        f.ellipse(kx, ky + 1, 6, 4, ol); f.ellipse(kx, ky + 1, 5, 3, C(232, 202, 112)); f.hline(kx - 4, kx + 4, ky + 3, C(196, 80, 70));
        f.hline(kx - 9, kx + 9, ky + 4, ol); f.hline(kx - 8, kx + 8, ky + 4, C(214, 180, 96)); f.set(kx - 2, ky - 1, C(250, 230, 160));
        f.rect(kx - 5, ky + 10, 11, 5, C(250, 210, 64)); f.hline(kx - 5, kx + 5, ky + 14, C(208, 160, 44));
    }
    int sweep = rowing ? wave(t, 6, 6.0f) : 0;
    if (c < 46) {
        f.line(bx + 30, by + 4, bx + 14 + sweep, by + 18, C(114, 72, 50));
        f.line(bx + 44, by + 4, bx + 60 - sweep, by + 18, C(114, 72, 50));
    } else f.line(bx + 10, by + 3, bx + 62, by + 3, C(114, 72, 50));
    Frame::Pts hull = { {bx, by + 6}, {bx + 70, by + 4}, {bx + 76, by - 2}, {bx + 72, by + 12}, {bx + 66, by + 19}, {bx + 6, by + 19} };
    f.poly(hull, ol);
    Frame::Pts in = { {bx + 2, by + 7}, {bx + 70, by + 5}, {bx + 74, by + 1}, {bx + 70, by + 12}, {bx + 65, by + 18}, {bx + 7, by + 18} };
    f.poly(in, wood);
    f.hline(bx + 2, bx + 70, by + 7, woodL);
    f.hline(bx + 4, bx + 69, by + 12, woodD); f.hline(bx + 6, bx + 66, by + 16, woodD);
    for (int k = 1; k < 4; ++k) f.vline(bx + k * 18, by + 8, by + 17, woodD);
    f.set(bx + 62, by + 9, C(220, 200, 160)); f.set(bx + 64, by + 9, C(220, 200, 160));
}
void LakeRound::drawWalkingKid(Frame& f, int x, int y, int facing, int step) {
    int h = 12 + (y - 116) * 10 / 82;
    h = std::max(10, std::min(22, h));
    uint8_t ol = C(OUT), skin = C(246, 204, 164), shirt = C(250, 210, 64), shorts = C(72, 100, 170), hat = C(232, 202, 112);
    int head = h * 3 / 10, body = h * 4 / 10, legs = h - head - body;
    int top = y - h;
    int sw = (step % 2) ? 1 : -1;
    f.vline(x - 1 + (facing == 2 ? 0 : sw), y - legs, y, ol); f.vline(x + 1 - (facing == 2 ? 0 : sw), y - legs, y, ol);
    f.rect(x - 2, y - legs - body / 3, 5, body / 3 + 1, shorts);
    int neckY = top + head + 3;
    f.vline(x - 1, top + head + 2, neckY, skin); f.vline(x, top + head + 2, neckY, skin); f.vline(x + 1, top + head + 2, neckY, skin);
    int shirtH = std::max(1, body - body / 3 - 3);
    f.rect(x - 3, neckY, 7, shirtH + 1, ol); f.rect(x - 2, neckY + 1, 5, shirtH, shirt);
    f.vline(x - 3, neckY + 1, neckY + 1 + shirtH / 2 + (step % 2), skin); f.vline(x + 3, neckY + 1, neckY + 2 + shirtH / 2 - (step % 2), skin);
    f.disc(x, top + head / 2 + 1, head / 2 + 1, ol); f.disc(x, top + head / 2 + 1, head / 2, skin);
    {
        int r2 = head / 2 + 1, cy2 = top + head / 2 + 1;
        for (int yy = cy2 + r2 / 2; yy <= cy2 + r2; ++yy) {
            int dy2 = yy - cy2, span = static_cast<int>(std::sqrt(static_cast<float>(std::max(0, r2 * r2 - dy2 * dy2))));
            f.hline(x - span, x + span, yy, skin);
        }
    }
    if (facing == 2) {
        int r = head / 2, cy = top + head / 2 + 1, hairBottom = cy - r / 3;
        for (int yy = top; yy <= hairBottom; ++yy) {
            int dy2 = yy - cy, span = static_cast<int>(std::sqrt(static_cast<float>(std::max(0, r * r - dy2 * dy2))));
            f.hline(x - span, x + span, yy, C(110, 70, 44));
        }
    }
    f.hline(x - head / 2 - 2, x + head / 2 + 2, top + 1, hat); f.hline(x - head / 2, x + head / 2, top, hat);
    if (facing != 2) f.set(x + (facing == 1 ? 1 : -1), top + head / 2 + 1, C(34, 26, 34));
}
void LakeRound::drawWorm(Frame& f, int t) {
    int sway = wave(t, 8, 1.0f);
    uint8_t ol = C(OUT), pink = C(236, 140, 156), pinkD = C(196, 96, 118), pinkL = C(250, 190, 200);
    f.ellipse(332, 262, 18, 5, C(96, 66, 46)); f.ellipse(332, 262, 15, 3, C(70, 46, 34));
    int hx = 334 + sway, hy = 230;
    float dx = hx - 332.0f, dy = hy + 4 - 258.0f, len = std::sqrt(dx * dx + dy * dy);
    int n = std::max(7, static_cast<int>(len / 4));
    for (int k = 0; k < n; ++k) {
        float u = static_cast<float>(k) / n;
        int x = 332 + static_cast<int>(dx * u) + sway * k / 6, y = 260 + static_cast<int>(dy * u) - static_cast<int>(std::sin(u * 3.14f) * (len > 40 ? 16 : 0));
        f.disc(x, y, 5, ol); f.disc(x, y, 4, pink); f.hline(x - 3, x + 3, y + 2, pinkD); f.set(x - 2, y - 2, pinkL);
    }
    Frame::Pts wing = { {hx - 4, hy}, {hx - 16, hy - 8}, {hx - 13, hy - 1}, {hx - 18, hy + 3}, {hx - 11, hy + 3}, {hx - 12, hy + 8}, {hx - 3, hy + 4} };
    f.poly(wing, C(110, 70, 140));
    f.disc(hx, hy, 7, ol); f.disc(hx, hy, 6, pink); f.disc(hx - 2, hy - 2, 2, pinkL);
    int ex = hx + 3;
    f.disc(ex, hy - 1, 3, ol); f.disc(ex, hy - 1, 2, C(250, 250, 250)); f.set(ex + (ex < hx ? -1 : 1), hy - 1, ol);
    if (t % 40 < 2) f.hline(ex - 2, ex + 2, hy - 1, ol);
    f.disc(hx + 1, hy - 9, 4, C(120, 20, 30)); f.disc(hx + 1, hy - 9, 3, C(214, 40, 56)); f.set(hx, hy - 10, C(240, 110, 120));
}
void LakeRound::drawForeground(Frame& f, const PlayStateReader&, int t) {
    drawBoat(f, t);
    int c = endStart_ >= 0 ? endFrame_ : -1;
    if (c >= 46 && c < 114) {
        int x, y; kidAt(x, y);
        int facing = c < 60 ? 1 : c < 106 ? 2 : 0;
        drawWalkingKid(f, x, y, facing, c / 2);
    }
    float w = wind(t);
    for (int k = 0; k < 6; ++k) {
        int x = 128 + k * 9, s = wave(t, 18, 1.0f, x * 0.35f) + static_cast<int>(w * 3);
        f.line(x, 53, x + s, 46 - k % 3, C(70, 130, 70));
        RGB p = (k % 3 == 0) ? RGB{ 236,200,240 } : (k % 3 == 1) ? RGB{ 250,230,120 } : RGB{ 244,170,186 };
        f.set(x + s, 45 - k % 3, C(p)); f.set(x + s - 1, 46 - k % 3, C(p)); f.set(x + s + 1, 46 - k % 3, C(p));
    }
    drawWindFx(f, t);
}
void LakeRound::drawNext(Frame& f, int piece, int t) {
    static const int BOB[16] = { 0,-1,-2,-2,-3,-2,0,1,2,3,4,4,5,4,2,1 };
    int wf = windFrame(t);
    int dy = BOB[t % 16], x0 = 235, y0 = 52 + dy;
    int c = endStart_ >= 0 ? endFrame_ : -1;
    bool sitting = c >= 122;
    if (sitting) {
        if (c >= 128) { drawSittingTeddy(f, t, c - 128); return; }
        float u = (c - 122) / 6.0f;
        y0 = 52 + static_cast<int>(27 * u * u);
    }
    uint8_t ol = C(90, 50, 36), fur = C(186, 118, 66), furL = C(206, 144, 90), muzzle = C(232, 184, 128);
    uint8_t wing = C(248, 248, 252), wingD = C(206, 210, 226), olW = C(150, 156, 180);
    int flap = (wf >= 0 && wf < 26) ? wave(t, 4, 3.0f) : (sitting && c < 128 ? 8 : 0);
    f.ellipse(x0 + 10, y0 + 40 - flap, 12, 18, olW); f.ellipse(x0 + 10, y0 + 40 - flap, 11, 17, wing);
    f.ellipse(x0 + 67, y0 + 40 - flap, 12, 18, olW); f.ellipse(x0 + 67, y0 + 40 - flap, 11, 17, wing);
    for (int k = 0; k < 4; ++k) { f.vline(x0 + 4 + k * 3, y0 + 34 - flap, y0 + 52 - flap, wingD); f.vline(x0 + 64 + k * 3, y0 + 34 - flap, y0 + 52 - flap, wingD); }
    int kick = wave(t, 16, 1.0f);
    f.ellipse(x0 + 38, y0 + 62, 22, 26, ol); f.ellipse(x0 + 38, y0 + 62, 21, 25, fur);
    f.ellipse(x0 + 38, y0 + 66, 13, 16, muzzle);
    f.ellipse(x0 + 26, y0 + 88 + kick, 7, 9, ol); f.ellipse(x0 + 26, y0 + 88 + kick, 6, 8, fur);
    f.ellipse(x0 + 50, y0 + 88 - kick, 7, 9, ol); f.ellipse(x0 + 50, y0 + 88 - kick, 6, 8, fur);
    f.ellipse(x0 + 25, y0 + 94 + kick, 5, 3, muzzle); f.ellipse(x0 + 51, y0 + 94 - kick, 5, 3, muzzle);
    f.disc(x0 + 25, y0 + 94 + kick, 1, C(150, 90, 60)); f.disc(x0 + 51, y0 + 94 - kick, 1, C(150, 90, 60));
    f.ellipse(x0 + 38, y0 - 4, 14, 3, C(206, 160, 40)); f.ellipse(x0 + 38, y0 - 4, 13, 2, C(252, 220, 96));
    f.ellipse(x0 + 38, y0 - 4, 9, 1, C(236, 240, 250));
    f.disc(x0 + 17, y0 + 8, 8, ol); f.disc(x0 + 17, y0 + 8, 7, fur); f.disc(x0 + 17, y0 + 9, 3, muzzle);
    f.disc(x0 + 59, y0 + 8, 8, ol); f.disc(x0 + 59, y0 + 8, 7, fur); f.disc(x0 + 59, y0 + 9, 3, muzzle);
    f.ellipse(x0 + 38, y0 + 23, 24, 20, ol); f.ellipse(x0 + 38, y0 + 23, 23, 19, fur);
    f.ellipse(x0 + 32, y0 + 15, 12, 7, furL);
    f.ellipse(x0 + 38, y0 + 31, 10, 7, muzzle);
    bool squint = wf >= 0 && wf < 20;
    if (squint) { f.hline(x0 + 26, x0 + 29, y0 + 23, C(30, 20, 20)); f.hline(x0 + 47, x0 + 50, y0 + 23, C(30, 20, 20)); }
    else {
        f.rect(x0 + 27, y0 + 22, 2, 3, C(30, 20, 20)); f.rect(x0 + 48, y0 + 22, 2, 3, C(30, 20, 20));
        f.set(x0 + 27, y0 + 22, C(250, 250, 250)); f.set(x0 + 48, y0 + 22, C(250, 250, 250));
    }
    f.rect(x0 + 36, y0 + 27, 5, 3, C(40, 24, 24)); f.vline(x0 + 38, y0 + 30, y0 + 32, C(40, 24, 24));
    f.set(x0 + 37, y0 + 33, C(40, 24, 24)); f.set(x0 + 39, y0 + 33, C(40, 24, 24));
    f.rect(x0 + 21, y0 + 29, 4, 2, C(236, 150, 150)); f.rect(x0 + 52, y0 + 29, 4, 2, C(236, 150, 150));
    uint8_t towel = C(250, 244, 228), towelD = C(232, 222, 200), panel = C(242, 234, 214), stripe = C(120, 160, 210);
    if (wf >= 6 && wf < 23) {
        int reach = wf < 12 ? (wf - 6) * 6 : wf < 17 ? 36 : 36 - (wf - 17) * 6;
        int ax = x0 + 70, ay = y0 + 46;
        for (int i = 0; i < 40; ++i) {
            int x = ax + i * (10 + reach) / 40;
            int wv = static_cast<int>(std::sin(i * 0.35f - t * 1.3f) * (2 + i / 8));
            f.vline(x, ay + wv - 10 + i / 6, ay + wv + 10 - i / 6, i % 6 < 1 ? towelD : towel);
            f.set(x, ay + wv + 7 - i / 6, stripe);
        }
        f.disc(ax, ay, 6, ol); f.disc(ax, ay, 5, fur);
        f.taper(x0 + 60, y0 + 40, ax, ay, 6, 5, fur);
        f.disc(x0 + 5, y0 + 49, 6, ol); f.disc(x0 + 5, y0 + 49, 5, fur);
        return;
    }
    f.rect(x0 + 5, y0 + 46, 64, 41, C(120, 110, 100));
    f.rect(x0 + 6, y0 + 47, 62, 39, towel);
    f.vline(x0 + 67, y0 + 47, y0 + 85, towelD); f.hline(x0 + 6, x0 + 67, y0 + 47, towelD);
    f.rect(x0 + 16, y0 + 55, 40, 20, panel);
    f.hline(x0 + 6, x0 + 67, y0 + 81, stripe); f.hline(x0 + 6, x0 + 67, y0 + 83, stripe);
    for (int x = x0 + 6; x < x0 + 68; x += 2) f.set(x, y0 + 86 + (wf >= 0 && wf < 26 ? wave(t + x, 3, 1.0f) : 0), towelD);
    if (wf >= 23 && wf < 26) {
        f.set(x0 + 22, y0 + 58, C(255, 255, 255)); f.set(x0 + 50, y0 + 70, C(255, 255, 255));
    }
    if (!sitting) ToyBlocks::nextPiece(f, state(), piece, x0 + 36, y0 + 65);
    f.disc(x0 + 5, y0 + 49, 6, ol); f.disc(x0 + 5, y0 + 49, 5, fur); f.disc(x0 + 4, y0 + 48, 2, furL);
    f.disc(x0 + 70, y0 + 49, 6, ol); f.disc(x0 + 70, y0 + 49, 5, fur); f.disc(x0 + 69, y0 + 48, 2, furL);
}
void LakeRound::drawSittingTeddy(Frame& f, int t, int k) {
    const int cx = 276, seat = 167;
    uint8_t ol = C(90, 50, 36), fur = C(186, 118, 66), furL = C(206, 144, 90), furD = C(150, 92, 54);
    uint8_t muzzle = C(232, 184, 128), muzzleD = C(206, 154, 100), pad = C(236, 170, 150), padD = C(196, 120, 110);
    int squash = (k < 2) ? 2 - k : 0;
    int breathe = (t / 12) % 2;
    int sway = (k > 4) ? wave(t, 40, 1.0f) : 0;
    f.ellipse(cx, seat, 19, 3, C(176, 128, 82));
    uint8_t wing = C(248, 248, 252), wingD = C(206, 210, 226), olW = C(150, 156, 180);
    f.ellipse(cx - 20, seat - 30, 7, 11, olW); f.ellipse(cx - 20, seat - 30, 6, 10, wing);
    f.ellipse(cx + 20, seat - 30, 7, 11, olW); f.ellipse(cx + 20, seat - 30, 6, 10, wing);
    for (int i = 0; i < 3; ++i) { f.vline(cx - 24 + i * 3, seat - 34, seat - 24, wingD); f.vline(cx + 18 + i * 3, seat - 34, seat - 24, wingD); }
    int by = seat - 15 + squash, bry = 15 - squash + breathe, brx = 17 + squash;
    f.ellipse(cx, by, brx + 1, bry + 1, ol); f.ellipse(cx, by, brx, bry, fur);
    f.ellipse(cx - 6, by - 6, 7, 5, furL);
    f.ellipse(cx, by + 3, 10, 9 - squash, muzzle); f.ellipse(cx + 3, by + 6, 6, 4, muzzleD);
    for (int s = -1; s <= 1; s += 2) {
        int lx = cx + s * 10, ly = seat + 2;
        f.ellipse(lx, ly - 3, 8, 6, ol); f.ellipse(lx, ly - 3, 7, 5, fur);
        f.ellipse(lx + s, ly + 1, 7, 7, ol); f.ellipse(lx + s, ly + 1, 6, 6, fur);
        f.ellipse(lx + s, ly + 2, 4, 4, pad);
        f.ellipse(lx + s + 1, ly + 3, 3, 2, padD);
        f.disc(lx + s - 3, ly - 3, 1, pad); f.disc(lx + s, ly - 4, 1, pad); f.disc(lx + s + 3, ly - 3, 1, pad);
        f.ellipse(lx, ly + 8, 7, 1, C(140, 92, 58));
    }
    uint8_t towel = C(250, 244, 228), towelD = C(222, 210, 186), stripe = C(120, 160, 210);
    f.rect(cx - 2, seat - 12, 20, 9, C(120, 110, 100)); f.rect(cx - 1, seat - 11, 18, 7, towel);
    f.hline(cx - 1, cx + 16, seat - 6, towelD); f.hline(cx - 1, cx + 16, seat - 8, stripe);
    for (int x = cx - 1; x < cx + 17; x += 2) f.set(x, seat - 4, towelD);
    f.disc(cx + 10, seat - 12, 5, ol); f.disc(cx + 10, seat - 12, 4, fur); f.disc(cx + 9, seat - 13, 1, furL);
    f.disc(cx - 14, seat - 6, 5, ol); f.disc(cx - 14, seat - 6, 4, fur); f.disc(cx - 15, seat - 7, 1, furL);
    f.taper(cx - 15, by - 9, cx - 14, seat - 8, 6, 5, fur);
    int hx = cx + sway, hy = seat - 42 + squash;
    f.disc(hx - 20, hy - 15, 8, ol); f.disc(hx - 20, hy - 15, 7, fur); f.disc(hx - 20, hy - 14, 3, muzzle);
    f.disc(hx + 20, hy - 15, 8, ol); f.disc(hx + 20, hy - 15, 7, fur); f.disc(hx + 20, hy - 14, 3, muzzle);
    f.ellipse(hx, hy, 24, 20, ol); f.ellipse(hx, hy, 23, 19, fur);
    f.ellipse(hx - 6, hy - 8, 12, 7, furL);
    f.ditherEllipse(hx + 15, hy + 6, 5, 8, furD);
    f.ellipse(hx, hy + 8, 10, 7, muzzle);
    bool blink = k > 4 && (t % 50) < 3;
    if (blink) { f.hline(hx - 12, hx - 8, hy, C(30, 20, 20)); f.hline(hx + 8, hx + 12, hy, C(30, 20, 20)); }
    else {
        f.set(hx - 12, hy, C(30, 20, 20)); f.hline(hx - 11, hx - 9, hy - 1, C(30, 20, 20)); f.set(hx - 8, hy, C(30, 20, 20));
        f.set(hx + 8, hy, C(30, 20, 20)); f.hline(hx + 9, hx + 11, hy - 1, C(30, 20, 20)); f.set(hx + 12, hy, C(30, 20, 20));
    }
    f.rect(hx - 2, hy + 4, 5, 3, C(40, 24, 24)); f.set(hx - 1, hy + 4, C(120, 90, 90));
    f.vline(hx, hy + 7, hy + 9, C(40, 24, 24));
    f.set(hx - 2, hy + 10, C(40, 24, 24)); f.set(hx - 1, hy + 11, C(40, 24, 24)); f.set(hx + 1, hy + 11, C(40, 24, 24)); f.set(hx + 2, hy + 10, C(40, 24, 24));
    f.rect(hx - 17, hy + 5, 4, 2, C(236, 150, 150)); f.rect(hx + 14, hy + 5, 4, 2, C(236, 150, 150));
    int tilt = k < 6 ? 2 : 1;
    for (int i = -13; i <= 13; ++i) {
        int y = hy - 25 + i * tilt / 13;
        f.set(hx + i, y - 1, C(252, 220, 96)); f.set(hx + i, y + 1, C(206, 160, 40));
        if (std::abs(i) > 9) f.set(hx + i, y, C(252, 220, 96));
    }
}
void LakeRound::drawHud(Frame& f, int, int t) {
    int lines = std::min(arrived_, goal());
    int x = 186, y = 212;
    uint8_t ol = C(OUT), frame = C(156, 104, 62), frameL = C(206, 156, 100), board = C(46, 74, 58), boardL = C(58, 88, 70);
    f.rect(x + 2, y + 2, 112, 50, C(70, 110, 60));
    f.rect(x, y, 112, 50, ol); f.rect(x + 1, y + 1, 110, 48, frame); f.hline(x + 1, x + 110, y + 1, frameL);
    f.rect(x + 5, y + 5, 102, 40, ol); f.rect(x + 6, y + 6, 100, 38, board);
    for (int k = 0; k < 6; ++k) f.hline(x + 10 + k * 15, x + 16 + k * 15, y + 30 + (k % 3) * 4, boardL);
    uint8_t chalk = C(226, 234, 226), chalkD = C(150, 170, 156);
    Font::draw(f, "ROUND 1", x + 10, y + 9, chalk);
    for (int k = 0; k < 5; ++k) f.rect(x + 66 + k * 6, y + 10, 3, 3, k == 0 ? C(250, 222, 110) : chalkD);
    f.disc(x + 99, y + 11, 2, chalkD); f.disc(x + 100, y + 10, 2, board);
    for (int i = 0; i < lines; ++i) {
        int g = i / 5, k = i % 5, tx = x + 30 + g * 22;
        if (k < 4) f.vline(tx + k * 3, y + 19, y + 25, chalk);
        else f.line(tx - 2, y + 25, tx + 11, y + 19, chalk);
    }
    for (int i = lines; i < goal(); ++i) f.set(x + 30 + (i / 5) * 22 + (i % 5) * 3, y + 25, chalkD);
    Font::center(f, "THE BOAT", x + 56, y + 33, lines >= goal() ? C(250, 222, 110) : chalkD);
    f.rect(x + 98, y + 44, 6, 2, C(240, 240, 240));
    if (memoryT_ >= 0 && t - memoryT_ < 20) {
        for (int k = 0; k < 6; ++k) {
            int sx = x + 30 + (k * 11 + t) % 54, sy = y + 31 + ((k * 7 + t) % 9);
            f.set(sx, sy, C(255, 250, 200)); f.set(sx - 1, sy, C(250, 222, 110)); f.set(sx + 1, sy, C(250, 222, 110));
        }
    }
}
void LakeRound::reset() {
    flyers_.clear(); arrived_ = 0; memoryT_ = -1; windStart_ = -1; gusts_ = 0;
    boatUp_ = 0; endStart_ = -1; endFrame_ = 0; boatX_ = 336; boatY_ = 150;
}
void LakeRound::specialEvent(int t) { if (endStart_ < 0 && (windFrame(t) < 0 || windFrame(t) > 30)) windStart_ = t; }
float LakeRound::wind(int t) const {
    int k = windFrame(t);
    if (k < 0 || k >= 20) return 0.0f;
    if (k < 6) return 0.3f + 0.7f * k / 5.0f;
    if (k <= 16) return 1.0f;
    static const float tail[3] = { 0.6f, 0.35f, 0.15f };
    return tail[k - 17];
}
void LakeRound::kidAt(int& x, int& y) const {
    float c = static_cast<float>(endFrame_);
    auto lerp = [](float a, float b, float u) { return a + (b - a) * std::max(0.0f, std::min(1.0f, u)); };
    if (c < 52) { float u = (c - 46) / 6.0f; x = static_cast<int>(lerp(boatX_ + 70, 436, u)); y = static_cast<int>(lerp(boatY_ + 4, 190, u) - std::sin(u * 3.14159f) * 10); }
    else if (c < 60) { x = static_cast<int>(lerp(436, 458, (c - 52) / 8.0f)); y = static_cast<int>(lerp(190, 198, (c - 52) / 8.0f)); }
    else if (c < 100) { x = 458; y = static_cast<int>(lerp(198, 134, (c - 60) / 40.0f)); }
    else if (c < 106) { x = 458; y = static_cast<int>(lerp(134, 113, (c - 100) / 6.0f)); }
    else { x = static_cast<int>(lerp(458, 441, (c - 106) / 8.0f)); y = 113; }
}
void LakeRound::update(const PlayStateReader&, int t) {
    int landed = 0;
    for (auto it = flyers_.begin(); it != flyers_.end();)
        if (t - it->start >= 7) { ++landed; it = flyers_.erase(it); } else ++it;
    arrived_ += landed / 4;
    if (arrived_ >= goal() && memoryT_ < 0) memoryT_ = t;
    if (memoryT_ >= 0 && endStart_ < 0 && t - memoryT_ >= 26) endStart_ = t;
    float w = wind(t);
    cloudX_ += 0.15f + 1.6f * w;
    if (w > 0) boatUp_ = std::min(24.0f, boatUp_ + 2.0f * w); else boatUp_ = std::max(0.0f, boatUp_ - 1.0f);
    if (endStart_ < 0) {
        boatX_ = 336.0f + 11.0f * std::sin(t * 6.2832f / 64);
        boatY_ = 150.0f - boatUp_ + std::sin(t * 6.2832f / 20);
    } else {
        endFrame_ = t - endStart_;
        if (endFrame_ == 0) { rowFromX_ = boatX_; rowFromY_ = boatY_; }
        float sx = rowFromX_, sy = rowFromY_;
        float u = std::max(0.0f, std::min(1.0f, (endFrame_ - 14) / 30.0f));
        u = u * u * (3 - 2 * u);
        boatX_ = sx + (348 - sx) * u;
        boatY_ = sy + (166 - sy) * u + (endFrame_ < 44 ? std::sin(t * 6.2832f / 20) : 0);
    }
}
void LakeRound::onLinesCleared(const std::vector<int>& rows, int totalLines, int t) {
    if (totalLines / 4 > gusts_ && totalLines < goal()) { gusts_ = totalLines / 4; windStart_ = t; }
    for (int r : rows)
        for (int k = 0; k < 4; ++k)
            flyers_.push_back({ static_cast<float>(boardX() + 12 + k * 18), static_cast<float>(boardY() + r * 8 + 4), t, (r + k) % 7 });
}
void LakeRound::drawEffects(Frame& f, const PlayStateReader&, int t) {
    drawWorm(f, t);
    int kx = static_cast<int>(boatX_) + 37, ky = static_cast<int>(boatY_) - 4;
    for (auto& fl : flyers_) {
        float u = (t - fl.start) / 7.0f;
        float cx = (fl.x0 + kx) / 2, cy = std::min(fl.y0, static_cast<float>(ky)) - 50;
        for (int tr = 2; tr >= 0; --tr) {
            float v = std::max(0.0f, u - tr * 0.06f);
            float x = (1 - v) * (1 - v) * fl.x0 + 2 * (1 - v) * v * cx + v * v * kx;
            float y = (1 - v) * (1 - v) * fl.y0 + 2 * (1 - v) * v * cy + v * v * ky;
            int size = std::max(2, static_cast<int>(7 - 4 * v)) - tr;
            const RGB* r = ToyBlocks::ramp(fl.type);
            if (size > 0) f.rect(static_cast<int>(x) - size / 2, static_cast<int>(y) - size / 2, size, size, C(r[tr == 0 ? 2 : 3]));
        }
    }
    if (memoryT_ >= 0 && t - memoryT_ < 20) {
        for (int k = 0; k < 10; ++k) {
            float a = k * 0.628f + (t - memoryT_) * 0.25f, rr = 34 + (t - memoryT_);
            int sx = kx + static_cast<int>(std::cos(a) * rr), sy = ky + 8 + static_cast<int>(std::sin(a) * rr * 0.4f);
            f.set(sx, sy, C(255, 255, 220)); f.set(sx - 1, sy, C(250, 222, 110)); f.set(sx + 1, sy, C(250, 222, 110));
            f.set(sx, sy - 1, C(250, 222, 110)); f.set(sx, sy + 1, C(250, 222, 110));
        }
    }
    if (memoryT_ >= 0 && t - memoryT_ < 26) {
        int m = t - memoryT_;
        float u = std::max(0.0f, std::min(1.0f, (m - 4) / 10.0f));
        int ix = 242 + static_cast<int>((34 - 242) * u), iy = 246 + static_cast<int>((236 - 246) * u) - static_cast<int>(std::sin(u * 3.14159f) * 70);
        int bounce = (m >= 14 && m < 18) ? (m % 2 ? 3 : 1) : 0;
        if (m >= 14) drawMemoryBook(f, 6, 226 - bounce);
        if (m < 15) {
            int tilt = (m / 2) % 2;
            uint8_t ol = C(OUT);
            f.poly({ {ix - 7, iy - 1 + tilt}, {ix + 7, iy - 1 - tilt}, {ix + 5, iy + 3}, {ix - 5, iy + 3} }, ol);
            f.poly({ {ix - 6, iy + tilt}, {ix + 6, iy - tilt}, {ix + 4, iy + 2}, {ix - 4, iy + 2} }, C(196, 146, 94));
            f.disc(ix, iy - 4, 2, C(232, 202, 112));
            f.set(ix - 9, iy + 2, C(255, 250, 200)); f.set(ix + 9, iy - 3, C(255, 250, 200));
        }
        if (m >= 14 && m < 24) {
            int r = (m - 14) * 3 + 6;
            for (int k = 0; k < 8; ++k) {
                float a = k * 0.785f;
                int sx = 34 + static_cast<int>(std::cos(a) * r), sy = 242 + static_cast<int>(std::sin(a) * r * 0.7f);
                f.set(sx, sy, C(255, 250, 200)); f.set(sx - 1, sy, C(250, 222, 110)); f.set(sx + 1, sy, C(250, 222, 110));
                f.set(sx, sy - 1, C(250, 222, 110)); f.set(sx, sy + 1, C(250, 222, 110));
            }
            if (m < 20) Font::center(f, "THE BOAT!", 34, 212 - (m - 14), C(252, 228, 152), C(OUT));
        }
    }
    int c = endStart_ >= 0 ? endFrame_ : -1;
    if (c >= 8 && c < 40) {
        int bx = 382, by = 66;
        f.rect(bx - 1, by - 1, 58, 22, C(OUT)); f.rect(bx, by, 56, 20, C(252, 250, 244));
        f.poly({ {bx + 44, by + 19}, {bx + 52, by + 19}, {436, 96} }, C(252, 250, 244));
        f.line(bx + 44, by + 20, 436, 96, C(OUT)); f.line(bx + 52, by + 20, 436, 96, C(OUT));
        Font::center(f, "COME IN!", bx + 28, by + 3, C(150, 58, 70));
        Font::center(f, "DINNER!", bx + 28, by + 11, C(150, 58, 70));
    }
    if (c >= FADE0) {
        float k = std::min(1.0f, (c - FADE0 + 1) / 10.0f);
        for (auto& col : palette()) col = mix(col, RGB{ 0, 0, 0 }, k);
    }
}
void LakeRound::drawWindFx(Frame& f, int t) {
    float w = wind(t);
    if (w <= 0) return;
    int n = static_cast<int>(18 * w);
    for (int k = 0; k < n; ++k) {
        int y = 20 + (k * 37) % 200, x = (k * 97 + t * 14) % 520 - 20, len = 6 + k % 6;
        f.hline(x, x + len, y, C(236, 244, 250));
    }
    for (int k = 0; k < n / 2; ++k) {
        int x = (k * 131 + t * 9) % 500 - 10, y = 40 + (k * 53 + static_cast<int>(std::sin(t * 0.5f + k) * 6)) % 160;
        f.set(x, y, k % 3 ? C(86, 158, 72) : C(244, 170, 186)); f.set(x + 1, y + (k % 2), k % 3 ? C(132, 194, 92) : C(250, 210, 220));
    }
    for (int k = 0; k < static_cast<int>(10 * w); ++k) {
        int x = 324 + (k * 41 + t * 3) % 96, y = 80 + (k * 29) % 120;
        f.hline(x, x + 2, y, C(250, 252, 255)); f.set(x + 1, y - 1, C(250, 252, 255));
    }
}
