#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include "ui/PixelArt.h"
#include "ui/PlayStateReader.h"
using namespace std;
class BoardOverlay {
public:
    static void draw(Frame& f, const PlayStateReader& g, int bx, int by, int cs, int t) {
        fog(f, g, bx, by, cs, t);
        for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x)
            if (!g.hidden(x, y) && g.marked(x, y) && g.at(x, y) >= 0) warning(f, bx + x * cs, by + y * cs, cs, t);
    }
private:
    static void fog(Frame& f, const PlayStateReader& g, int bx, int by, int cs, int t) {
        const int W = PlayStateReader::kCols * cs, H = PlayStateReader::kRows * cs;
        vector<char> in(static_cast<size_t>(W * H), 0), m(static_cast<size_t>(W * H), 0);
        bool any = false;
        for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) if (g.hidden(x / cs, y / cs)) { in[static_cast<size_t>(y * W + x)] = 1; any = true; }
        if (!any) return;
        auto at = [&](const vector<char>& v, int x, int y) { return x >= 0 && x < W && y >= 0 && y < H && v[static_cast<size_t>(y * W + x)]; };
        const int per = cs >= 8 ? 12 : 9, rad = cs >= 8 ? 7 : 5, deep = rad, ph = t / 3;
        auto bump = [&](int v) { int k = ((v + ph) % per + per) % per - per / 2; return static_cast<int>(rad - sqrt(static_cast<double>(rad * rad - k * k)) + 0.5); };
        auto run = [&](int x, int y, int dx, int dy) { int n = 0; while (n <= deep && at(in, x + dx * (n + 1), y + dy * (n + 1))) ++n; return n; };
        int x0 = W, y0 = H, x1 = -1, y1 = -1;
        for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
            if (!at(in, x, y)) continue;
            if (run(x, y, 0, -1) >= bump(x) && run(x, y, 0, 1) >= bump(x + 4) && run(x, y, -1, 0) >= bump(y) && run(x, y, 1, 0) >= bump(y + 4)) {
                m[static_cast<size_t>(y * W + x)] = 1;
                x0 = min(x0, x); y0 = min(y0, y); x1 = max(x1, x); y1 = max(y1, y);
            }
        }
        uint8_t ol = C(50, 48, 80), dk = C(92, 92, 130), md = C(138, 140, 174), lt = C(186, 188, 216), hi = C(228, 229, 242);
        for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
            if (at(in, x, y) && !at(m, x, y)) f.set(bx + x, by + y, ol);
            if (!at(m, x, y)) continue;
            uint8_t c = md;
            if (!at(m, x + 3, y + 3)) c = dk;
            else if (!at(m, x - 2, y - 2)) c = lt;
            if (!at(m, x - 1, y - 1) && !at(m, x, y - 2)) c = hi;
            if (!at(m, x - 1, y) || !at(m, x + 1, y) || !at(m, x, y - 1) || !at(m, x, y + 1)) c = at(m, x - 1, y - 1) ? ol : dk;
            f.set(bx + x, by + y, c);
        }
        if (x1 - x0 < 16 || y1 - y0 < 16) return;
        for (int j = 0; j < 3; ++j) {
            int cx = x0 + 8 + ((j * 13 + ph) % max(1, x1 - x0 - 16)), cy = y0 + 10 + j * (y1 - y0 - 14) / 3;
            for (int i = -5; i <= 5; ++i) {
                int yy = cy - static_cast<int>(2.0 * sqrt(1.0 - i * i / 30.0) + 0.5);
                if (at(m, cx + i, yy) && at(m, cx + i - 1, yy) && at(m, cx + i + 1, yy) && at(m, cx + i, yy - 1) && at(m, cx + i, yy + 1)) f.set(bx + cx + i, by + yy, lt);
            }
        }
    }
    static void warning(Frame& f, int px, int py, int cs, int t) {
        uint8_t red = C(255, 70, 70), white = C(250, 240, 240);
        if ((t / 3) % 2 == 0) for (int j = 0; j < cs; ++j) for (int i = 0; i < cs; ++i) if ((i + j) % 2 == 0) f.set(px + i, py + j, white);
        f.hline(px, px + cs - 1, py, red); f.hline(px, px + cs - 1, py + cs - 1, red);
        f.vline(px, py, py + cs - 1, red); f.vline(px + cs - 1, py, py + cs - 1, red);
    }
};
