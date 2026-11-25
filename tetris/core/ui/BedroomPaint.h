#pragma once
#include "ui/PixelArt.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
namespace BedroomPaint {
struct Pt { float x, y; };
struct Cv {
    int w, h, fw, ox = 0;
    std::vector<std::string> r;
    Cv(int fw_, int n, int h_) : w(fw_ * n), h(h_), fw(fw_), r(h_, std::string(fw_ * n, '.')) {}
    void frame(int k) { ox = k * fw; }
    void set(int x, int y, char c) { if (x >= 0 && x < fw && y >= 0 && y < h) r[y][ox + x] = c; }
    char get(int x, int y) const { return (x >= 0 && x < fw && y >= 0 && y < h) ? r[y][ox + x] : '.'; }
    void rect(int x0, int y0, int x1, int y1, char c) { for (int y = y0; y <= y1; ++y) for (int x = x0; x <= x1; ++x) set(x, y, c); }
    void line(int x0, int y0, int x1, int y1, char c) {
        int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, e = dx + dy;
        for (;;) {
            set(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * e;
            if (e2 >= dy) { e += dy; x0 += sx; }
            if (e2 <= dx) { e += dx; y0 += sy; }
        }
    }
    void ell(float cx, float cy, float rx, float ry, char c) {
        for (int y = static_cast<int>(std::floor(cy - ry)); y <= static_cast<int>(std::ceil(cy + ry)); ++y)
            for (int x = static_cast<int>(std::floor(cx - rx)); x <= static_cast<int>(std::ceil(cx + rx)); ++x) {
                float a = (x + 0.5f - cx) / rx, b = (y + 0.5f - cy) / ry;
                if (a * a + b * b <= 1.0f) set(x, y, c);
            }
    }
    void poly(const std::vector<Pt>& p, char c) {
        float lo = p[0].y, hi = p[0].y;
        for (auto& q : p) { lo = std::min(lo, q.y); hi = std::max(hi, q.y); }
        for (int y = static_cast<int>(std::floor(lo)); y <= static_cast<int>(std::ceil(hi)); ++y) {
            float yc = y + 0.5f;
            std::vector<float> xs;
            for (size_t i = 0; i < p.size(); ++i) {
                const Pt& a = p[i];
                const Pt& b = p[(i + 1) % p.size()];
                if ((a.y <= yc && b.y > yc) || (b.y <= yc && a.y > yc)) xs.push_back(a.x + (yc - a.y) * (b.x - a.x) / (b.y - a.y));
            }
            std::sort(xs.begin(), xs.end());
            for (size_t k = 0; k + 1 < xs.size(); k += 2)
                for (int x = static_cast<int>(std::ceil(xs[k] - 0.5f)); x < static_cast<int>(std::ceil(xs[k + 1] - 0.5f)); ++x) set(x, y, c);
        }
    }
    void outline(char c) {
        std::vector<std::pair<int, int>> todo;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < fw; ++x) {
                if (get(x, y) != '.') continue;
                const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
                for (int i = 0; i < 4; ++i) {
                    char n = get(x + dx[i], y + dy[i]);
                    if (n != '.' && n != c) { todo.push_back({x, y}); break; }
                }
            }
        for (auto& t : todo) set(t.first, t.second, c);
    }
};
inline const std::vector<std::pair<char, RGB>>& pal() {
    static const std::vector<std::pair<char, RGB>> p = {
        {'O', {30, 22, 34}}, {'B', {92, 66, 58}}, {'L', {124, 92, 74}}, {'R', {112, 82, 68}}, {'P', {72, 54, 56}},
        {'G', {204, 164, 84}}, {'F', {8, 6, 14}}, {'S', {132, 100, 80}}, {'T', {100, 76, 64}},
        {'K', {32, 32, 46}}, {'M', {58, 58, 82}}, {'N', {18, 18, 28}}, {'Y', {240, 200, 40}}, {'W', {236, 232, 226}},
        {'D', {130, 28, 44}}, {'E', {84, 44, 70}}, {'Z', {10, 10, 16}}, {'V', {176, 62, 130}},
        {'a', {252, 226, 142}}, {'b', {224, 190, 100}}, {'c', {156, 142, 124}}, {'d', {124, 112, 98}},
        {'e', {128, 128, 150}}, {'f', {104, 104, 126}}, {'g', {146, 104, 74}}, {'h', {86, 58, 58}},
        {'i', {58, 40, 46}}, {'y', {60, 40, 44}},
        {'s', {206, 206, 224}}, {'t', {166, 166, 194}}, {'v', {96, 68, 60}}, {'w', {70, 50, 50}}, {'x', {150, 112, 80}},
        {'q', {46, 66, 124}}, {'r', {86, 112, 176}}, {'n', {204, 204, 228}}, {'m', {170, 170, 200}},
        {'H', {104, 66, 42}}, {'I', {66, 42, 30}}, {'k', {248, 214, 180}}, {'C', {232, 140, 136}},
        {'J', {122, 146, 206}}, {'U', {96, 122, 190}}, {'X', {40, 30, 46}}, {'z', {255, 240, 170}}
    };
    return p;
}
inline Sprite make(const char* name, const Cv& c) { return Sprite(name, c.r, pal()); }
inline Sprite closet() {
    Cv c(90, 5, 173);
    const float LX = 21, RX = 88, TL = 21, TR = 2, BL = 144, BR = 172;
    const int leaf[5] = {0, 4, 12, 17, 19};
    for (int k = 0; k < 5; ++k) {
        c.frame(k);
        for (int x = 21; x <= 88; ++x) {
            float t = TL + (x + 0.5f - LX) * (TR - TL) / (RX - LX);
            float b = BL + (x + 0.5f - LX) * (BR - BL) / (RX - LX);
            for (int y = static_cast<int>(t); y <= static_cast<int>(b) && y < 173; ++y) {
                float u = (x + 0.5f - LX) / (RX - LX), v = (y + 0.5f - t) / (b - t);
                float du = 1.0f / 67, dv = 1.0f / (b - t);
                auto in = [&](float u0, float u1, float v0, float v1) { return u >= u0 && u <= u1 && v >= v0 && v <= v1; };
                auto panel = [&](float u0, float u1, float v0, float v1) { return in(u0, u1, v0, v1) && !in(u0 + du, u1 - du, v0 + dv, v1 - dv); };
                char ch = 'B';
                if (y < t + 3) ch = 'L';
                else if (x >= 83) ch = 'R';
                else if (x == 49) ch = 'O';
                else if (k > 0 && x < 49) {
                    if (x < 25) ch = 'B';
                    else if (y <= b - 3) ch = 'F';
                    else ch = 'O';
                } else if ((k == 0 && (panel(0.10f, 0.36f, 0.12f, 0.46f) || panel(0.10f, 0.36f, 0.54f, 0.92f))) ||
                           panel(0.50f, 0.84f, 0.12f, 0.46f) || panel(0.50f, 0.84f, 0.54f, 0.92f)) ch = 'P';
                c.set(x, y, ch);
            }
        }
        c.rect(52, 88, 53, 95, 'G');
        if (k == 0) c.rect(46, 86, 47, 92, 'G');
        if (k > 0) {
            float w = static_cast<float>(leaf[k]);
            c.poly({{20, 21}, {20 - w, 22}, {20 - w, 147 + w / 6}, {20, 145}}, k == 1 ? 'T' : 'S');
            if (k > 1) c.line(static_cast<int>(20 - w) + 2, 26, static_cast<int>(20 - w) + 2, 142, 'L');
        }
        c.outline('O');
    }
    return make("closet", c);
}
inline void wolfHead(Cv& c, int k) {
    static const int OP[9] = {0, 1, 2, 3, 4, 3, 2, 1, 0};
    int o = OP[k];
    c.poly({{24, 22}, {31, 1}, {39, 22}}, 'K');
    c.poly({{29, 18}, {31, 6}, {34, 18}}, 'E');
    c.poly({{42, 22}, {49, 3}, {56, 22}}, 'K');
    c.poly({{46, 18}, {49, 8}, {52, 18}}, 'E');
    c.poly({{22, 22}, {57, 20}, {60, 34}, {57, 46}, {30, 50}, {22, 48}}, 'K');
    c.poly({{0, 35}, {24, 26}, {32, 26}, {32, 38}, {3, 38}}, 'K');
    float fo = static_cast<float>(o);
    c.poly({{3, 40 + fo}, {32, 40 + fo}, {32, 46 + fo}, {8, 46 + fo}}, 'K');
    if (o > 0) c.rect(6, 39, 30, 39 + o, 'D');
    for (int x = 6; x <= 29; x += 2) {
        c.set(x, 39, 'W');
        if (o > 0) { c.set(x, 40, 'W'); c.set(x + 1, 38 + o, 'W'); c.set(x + 1, 39 + o, 'W'); }
    }
    if (o == 0) c.rect(5, 39, 29, 39, 'W');
    c.line(24, 22, 31, 1, 'M');
    c.line(42, 22, 49, 3, 'M');
    c.line(0, 35, 24, 26, 'M');
    c.line(26, 23, 31, 24, 'N');
    if (k == 8) {
        c.line(27, 26, 30, 26, 'N');
        c.line(38, 25, 41, 25, 'N');
    } else {
        c.rect(27, 25, 29, 26, 'Y');
        c.rect(38, 24, 40, 25, 'Y');
        c.set(28, 25, 'Z');
        c.set(39, 24, 'Z');
    }
    c.rect(0, 34, 2, 36, 'Z');
    for (int x = 40; x < 70; x += 4) c.poly({{static_cast<float>(x), 50}, {static_cast<float>(x) + 2, 44}, {static_cast<float>(x) + 4, 50}}, 'K');
}
inline Sprite wolfIn() {
    Cv c(71, 9, 119);
    for (int k = 0; k < 9; ++k) {
        c.frame(k);
        c.poly({{12, 44}, {62, 44}, {69, 118}, {12, 118}}, 'K');
        for (int x : {10, 14, 40, 44, 48, 52, 56, 60, 64}) c.poly({{static_cast<float>(x), 44}, {static_cast<float>(x) + 2, 38}, {static_cast<float>(x) + 4, 44}}, 'K');
        c.line(12, 46, 12, 118, 'M');
        wolfHead(c, k);
        c.outline('N');
    }
    return make("wolf_in_closet", c);
}
inline Sprite wolfHeadOut() {
    Cv c(57, 8, 50);
    for (int k = 0; k < 8; ++k) {
        c.frame(k);
        wolfHead(c, k);
        c.outline('N');
    }
    return make("wolf_head_out", c);
}
inline Sprite wolfClaws() {
    Cv c(20, 4, 21);
    const int sy[3] = {6, 10, 14};
    for (int k = 0; k < 4; ++k) {
        c.frame(k);
        static const int SP[4] = {0, 1, 2, 1};
        int s = SP[k];
        int ty[3] = {3 - s, 10, 17 + s};
        c.ell(7, 10, 6, 8, 'K');
        c.line(3, 5, 3, 14, 'M');
        for (int i = 0; i < 3; ++i) c.line(12, sy[i], 19, ty[i], 'V');
        c.outline('N');
    }
    return make("wolf_claws", c);
}
inline Sprite lampTable() {
    Cv c(44, 2, 66);
    for (int k = 0; k < 2; ++k) {
        c.frame(k);
        bool on = k == 0;
        c.poly({{4, 23}, {40, 23}, {43, 31}, {0, 31}}, 'g');
        c.rect(4, 31, 43, 65, 'h');
        c.poly({{0, 31}, {4, 31}, {4, 65}, {0, 65}}, 'y');
        for (int x : {10, 16, 22, 28, 34, 40}) c.line(x, 34, x, 64, 'i');
        c.rect(1, 40, 2, 46, 'G');
        c.rect(18, 15, 19, 27, 'e');
        c.ell(19, 27, 7, 2, 'f');
        c.poly({{10, 1}, {29, 1}, {32, 14}, {7, 14}}, on ? 'a' : 'c');
        c.poly({{20, 1}, {29, 1}, {32, 14}, {22, 14}}, on ? 'b' : 'd');
        c.outline('O');
    }
    return make("lamp_table", c);
}
inline Sprite bedFrame() {
    Cv c(131, 1, 101);
    c.rect(28, 1, 102, 9, 'L');
    c.poly({{29, 10}, {102, 10}, {125, 73}, {6, 73}}, 's');
    c.line(29, 10, 6, 73, 't');
    c.line(102, 10, 125, 73, 't');
    c.line(6, 73, 125, 73, 't');
    c.poly({{6, 74}, {15, 74}, {12, 100}, {0, 100}}, 'O');
    c.poly({{116, 74}, {125, 74}, {131, 100}, {120, 100}}, 'O');
    return make("bed_frame", c);
}
inline Sprite headboard() {
    Cv c(131, 1, 47);
    c.rect(0, 12, 130, 46, 'v');
    c.rect(0, 12, 130, 14, 'L');
    c.rect(2, 6, 7, 46, 'v');
    c.rect(124, 6, 129, 46, 'v');
    c.ell(4.5f, 5, 3.5f, 3, 'x');
    c.ell(126.5f, 5, 3.5f, 3, 'x');
    for (int x : {18, 38, 58, 78, 98, 118}) c.line(x, 18, x, 45, 'w');
    c.outline('O');
    return make("headboard", c);
}
inline void stars(Cv& c, int ymax) {
    static const int S[][2] = {{20, 6}, {44, 10}, {72, 5}, {96, 12}, {34, 24}, {60, 20}, {84, 28}, {50, 36}, {78, 38}, {14, 14}};
    for (auto& s : S) if (s[1] < ymax) c.set(s[0], s[1], 'z');
}
inline Sprite kidBed() {
    Cv c(122, 11, 67);
    for (int k = 0; k < 11; ++k) {
        c.frame(k);
        c.rect(34, 47, 89, 60, 'n');
        c.rect(32, 49, 91, 58, 'n');
        c.rect(34, 61, 89, 61, 'm');
        bool flat = k == 10;
        c.poly({{23, 0}, {99, 0}, {flat ? 111.0f : 115.0f, flat ? 22.0f : 42.0f}, {flat ? 11.0f : 7.0f, flat ? 22.0f : 42.0f}}, 'q');
        c.rect(flat ? 11 : 7, flat ? 23 : 43, flat ? 111 : 115, flat ? 23 : 44, 's');
        if (k <= 5 || k == 9) {
            c.ell(56, 16, 4, 3, 'r');
            c.line(56, 19, 50, 38, 'r');
            c.line(58, 19, 66, 38, 'r');
        }
        stars(c, flat ? 20 : 42);
        if (k <= 4) {
            int cx = k == 2 ? 56 : (k == 4 ? 68 : 62);
            c.ell(cx, 54 + (k == 1 ? 1 : 0), 11, 11, 'H');
            c.rect(cx - 14, 52, cx - 12, 56, 'k');
            c.rect(cx + 12, 52, cx + 14, 56, 'k');
            c.line(cx + 2, 43, cx + 4, 40, 'I');
            c.line(cx - 4, 52, cx - 5, 55, 'I');
            c.line(cx - 4, 57, cx, 58, 'I');
            c.line(cx, 58, cx + 3, 55, 'I');
        } else if (k == 5) {
            c.ell(62, 42, 11, 11, 'H');
            c.rect(46, 42, 49, 46, 'k');
            c.rect(75, 42, 78, 46, 'k');
            c.rect(42, 46, 46, 49, 'J');
            c.rect(78, 46, 82, 49, 'J');
            c.line(64, 31, 66, 28, 'I');
        } else if (k <= 8) {
            int dx = k == 7 ? -2 : (k == 8 ? 2 : 0);
            c.ell(62 + dx, 50, 15, 11, 'r');
            c.ell(62 + dx, 52, 12, 8, 'q');
        } else if (k == 9) {
            c.poly({{44, 36}, {50, 32}, {73, 32}, {79, 36}, {79, 66}, {44, 66}}, 'J');
            c.line(62, 34, 62, 66, 'U');
            c.ell(62, 23, 11, 11, 'H');
            c.rect(48, 22, 50, 26, 'k');
            c.rect(74, 22, 76, 26, 'k');
            c.line(64, 12, 66, 9, 'I');
        } else {
            c.ell(62, 54, 11, 2, 'm');
        }
        c.outline('O');
    }
    return make("kid_in_bed", c);
}
inline Sprite kidCrawl() {
    Cv c(118, 8, 62);
    for (int k = 0; k < 8; ++k) {
        c.frame(k);
        c.rect(30, 59, 88, 61, 'n');
        c.poly({{22, 0}, {97, 0}, {117, 58}, {2, 58}}, 'q');
        stars(c, 56);
        float cy = 47 - 5.6f * k, rx = 14 - 0.55f * k, ry = 7 - 0.3f * k;
        c.ell(59, cy, rx, ry, 'r');
        c.outline('O');
    }
    return make("kid_crawl", c);
}
inline void pjBack(Cv& c, int ph) {
    c.ell(16, 13, 8, 8, 'H');
    c.set(20, 3, 'I');
    c.set(21, 2, 'I');
    c.set(21, 3, 'I');
    c.set(17, 12, 'I');
    c.rect(15, 21, 17, 22, 'k');
    c.rect(9, 23, 23, 35, 'J');
    c.rect(9, 32, 23, 35, 'U');
    int swL = ph == 1 ? 1 : (ph == 3 ? -1 : 0);
    c.rect(7, 24 + swL, 8, 33 + swL, 'U');
    c.rect(24, 24 - swL, 25, 33 - swL, 'U');
    c.rect(11, 36, 12, ph == 3 ? 42 : 45, 'X');
    c.rect(19, 36, 20, ph == 1 ? 42 : 45, 'X');
}
inline Sprite pjWalk() {
    Cv c(32, 5, 46);
    for (int k = 0; k < 5; ++k) {
        c.frame(k);
        pjBack(c, k);
        c.outline('X');
    }
    return make("pj_walk", c);
}
inline Sprite pjTurn() {
    Cv c(32, 3, 46);
    c.frame(0);
    c.ell(16, 13, 8, 8, 'k');
    c.poly({{8, 13}, {9, 6}, {16, 4}, {23, 6}, {24, 13}, {21, 9}, {16, 8}, {11, 9}}, 'H');
    c.set(20, 3, 'I');
    c.set(21, 2, 'I');
    c.rect(12, 14, 12, 15, 'Z');
    c.rect(20, 14, 20, 15, 'Z');
    c.set(10, 17, 'C');
    c.set(22, 17, 'C');
    c.rect(15, 19, 17, 19, 'D');
    c.rect(9, 23, 23, 35, 'J');
    c.rect(9, 32, 23, 35, 'U');
    c.rect(7, 24, 8, 33, 'U');
    c.rect(24, 24, 25, 33, 'U');
    c.rect(11, 36, 12, 45, 'X');
    c.rect(19, 36, 20, 45, 'X');
    c.outline('X');
    c.frame(1);
    c.ell(17, 13, 7, 8, 'k');
    c.poly({{9, 13}, {10, 6}, {17, 4}, {20, 7}, {17, 10}, {14, 18}, {9, 17}}, 'H');
    c.set(20, 3, 'I');
    c.set(21, 2, 'I');
    c.set(21, 14, 'Z');
    c.set(21, 17, 'C');
    c.rect(12, 23, 20, 35, 'J');
    c.rect(12, 32, 20, 35, 'U');
    c.rect(15, 24, 17, 34, 'U');
    c.rect(13, 36, 15, 45, 'X');
    c.rect(17, 36, 19, 45, 'X');
    c.outline('X');
    c.frame(2);
    pjBack(c, 0);
    c.outline('X');
    return make("pj_turn", c);
}
}
