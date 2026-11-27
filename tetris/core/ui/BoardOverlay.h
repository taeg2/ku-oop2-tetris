#pragma once
#include "ui/PixelArt.h"
#include "ui/PlayStateReader.h"
using namespace std;
class BoardOverlay {
public:
    static void draw(Frame& f, const PlayStateReader& g, int bx, int by, int cs, int t) {
        for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x) {
            int px = bx + x * cs, py = by + y * cs;
            if (g.hidden(x, y)) fog(f, g, x, y, px, py, cs, t);
            else if (g.marked(x, y) && g.at(x, y) >= 0) warning(f, px, py, cs, t);
        }
    }
private:
    static void fog(Frame& f, const PlayStateReader& g, int x, int y, int px, int py, int cs, int t) {
        uint8_t base = C(150, 160, 184), light = C(186, 196, 216), edge = C(112, 122, 148);
        for (int j = 0; j < cs; ++j) for (int i = 0; i < cs; ++i) f.set(px + i, py + j, (i + j + t / 3) % 4 == 0 ? light : base);
        if (!g.hidden(x, y - 1)) f.hline(px, px + cs - 1, py, edge);
        if (!g.hidden(x, y + 1)) f.hline(px, px + cs - 1, py + cs - 1, edge);
        if (!g.hidden(x - 1, y)) f.vline(px, py, py + cs - 1, edge);
        if (!g.hidden(x + 1, y)) f.vline(px + cs - 1, py, py + cs - 1, edge);
    }
    static void warning(Frame& f, int px, int py, int cs, int t) {
        uint8_t red = C(255, 70, 70), white = C(250, 240, 240);
        if ((t / 3) % 2 == 0) for (int j = 0; j < cs; ++j) for (int i = 0; i < cs; ++i) if ((i + j) % 2 == 0) f.set(px + i, py + j, white);
        f.hline(px, px + cs - 1, py, red); f.hline(px, px + cs - 1, py + cs - 1, red);
        f.vline(px, py, py + cs - 1, red); f.vline(px + cs - 1, py, py + cs - 1, red);
    }
};
