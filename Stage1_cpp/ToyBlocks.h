// ToyBlocks.h - Stage 1 blocks: painted wooden toy blocks with a little peg (drawn with code).
#pragma once
#include "Art.h"
#include "Tetris.h"

class ToyBlocks {
public:
    // 5-shade ramp per piece, dark -> highlight (shadows cooler, highlights warmer)
    static const RGB* ramp(int type) {
        static const RGB R[8][5] = {
            {{ 36, 86,123}, { 58,136,174}, { 90,200,230}, {146,239,255}, {215,252,255}},   // I cyan
            {{132,133, 25}, {184,171, 43}, {240,206, 70}, {255,218,120}, {255,232,189}},   // O yellow
            {{ 89, 33, 93}, {129, 56,144}, {170, 90,200}, {201,145,231}, {240,226,251}},   // T purple
            {{ 33, 93, 34}, { 63,144, 56}, {110,200, 90}, {167,231,145}, {234,251,226}},   // S green
            {{123, 50, 30}, {174, 63, 50}, {230, 80, 80}, {255,135,143}, {255,203,211}},   // Z red
            {{ 30, 30,113}, { 50, 62,164}, { 80,110,220}, {136,169,251}, {207,225,255}},   // J blue
            {{133,100, 20}, {184,126, 35}, {240,150, 60}, {255,172,110}, {255,205,179}},   // L orange
            {{ 27, 24, 33}, { 70, 64, 84}, {120,116,140}, {171,171,171}, {191,191,191}} };   // dead grey
        return R[type < 0 || type > 7 ? 7 : type];
    }

    // 8x8 board cell
    static void cell8(Frame& f, int x, int y, int type) {
        const RGB* r = ramp(type);
        uint8_t c0 = C(r[0]), c1 = C(r[1]), c2 = C(r[2]), c3 = C(r[3]), c4 = C(r[4]);
        for (int j = 0; j < 8; ++j) for (int i = 0; i < 8; ++i) {
            uint8_t col = c2;
            if (i == 7 || j == 7) col = c0;
            else if (i == 6 || j == 6) col = c1;
            else if (i == 0 || j == 0) col = c3;
            f.set(x + i, y + j, col);
        }
        f.set(x + 1, y + 1, c4); f.set(x + 2, y + 1, c4); f.set(x + 1, y + 2, c4);   // sheen
        f.set(x, y, c2); f.set(x + 7, y, c1); f.set(x, y + 7, c1);                     // soft corners
        f.set(x + 3, y + 3, c4); f.set(x + 4, y + 3, c4); f.set(x + 3, y + 4, c3); f.set(x + 4, y + 4, c1); // peg
    }
    // landing preview: dotted outline
    static void ghost8(Frame& f, int x, int y, int type) {
        const RGB* r = ramp(type);
        uint8_t c3 = C(r[3]), c2 = C(r[2]);
        for (int i = 0; i < 8; ++i) {
            if ((i + 0) % 2 == 0) { f.set(x + i, y, c3); f.set(x, y + i, c3); }
            if ((i + 7) % 2 == 0) { f.set(x + i, y + 7, c3); f.set(x + 7, y + i, c3); }
        }
        f.set(x + 3, y + 3, c2); f.set(x + 4, y + 4, c2);
    }
    // 6x6 cell for NEXT
    static void cell6(Frame& f, int x, int y, int type) {
        const RGB* r = ramp(type);
        uint8_t c0 = C(r[0]), c1 = C(r[1]), c2 = C(r[2]), c3 = C(r[3]), c4 = C(r[4]);
        for (int j = 0; j < 6; ++j) for (int i = 0; i < 6; ++i)
            f.set(x + i, y + j, (i == 5 || j == 5) ? c0 : (i == 0 || j == 0) ? c3 : c2);
        f.set(x + 1, y + 1, c4); f.set(x + 2, y + 2, c3); f.set(x + 3, y + 3, c1);
        f.set(x + 5, y, c1); f.set(x, y + 5, c1);
    }
    // NEXT piece centered on (cx, cy) with 6 px cells
    static void nextPiece(Frame& f, int type, int cx, int cy) {
        auto cs = Tetris::cells(type, 0);
        int minx = 9, maxx = -1, miny = 9, maxy = -1;
        for (auto& c : cs) { minx = std::min(minx, c.first); maxx = std::max(maxx, c.first); miny = std::min(miny, c.second); maxy = std::max(maxy, c.second); }
        int w = (maxx - minx + 1) * 6, h = (maxy - miny + 1) * 6;
        for (auto& c : cs) cell6(f, cx - w / 2 + (c.first - minx) * 6, cy - h / 2 + (c.second - miny) * 6, type);
    }
    // full rows flash: white, yellow, white, before they disappear
    static void flashRows(Frame& f, const Tetris& g, int bx, int by, int k) {
        uint8_t c = (k == 1) ? C(255, 236, 140) : C(250, 248, 255), d = C(220, 210, 160);
        for (int y : g.clearingRows())
            for (int j = 0; j < 8; ++j) for (int i = 0; i < 80; ++i)
                f.set(bx + i, by + y * 8 + j, (k == 2 && ((i + j) & 1)) ? d : c);
    }
    // stack + ghost + falling piece inside the 80x160 board at (bx, by)
    static void board(Frame& f, const Tetris& g, int bx, int by, bool piece = true) {
        for (int y = 0; y < Tetris::ROWS; ++y) for (int x = 0; x < Tetris::COLS; ++x)
            if (g.at(x, y) >= 0) cell8(f, bx + x * 8, by + y * 8, g.over() ? 7 : g.at(x, y));
        if (g.over() || g.clearing() || !piece) return;
        int gy = g.ghostY();
        for (auto& c : Tetris::cells(g.current(), g.rotation())) {
            int x = g.px() + c.first;
            if (gy + c.second >= 0) ghost8(f, bx + x * 8, by + (gy + c.second) * 8, g.current());
        }
        for (auto& c : Tetris::cells(g.current(), g.rotation())) {
            int x = g.px() + c.first, y = g.py() + c.second;
            if (y >= 0) cell8(f, bx + x * 8, by + y * 8, g.current());
        }
    }
};
