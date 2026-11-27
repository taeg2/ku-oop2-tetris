#include "ui/RoundScene.h"
#include <algorithm>
#include <cmath>
#include "ui/PixelFont.h"
using namespace std;

static const RGB OUTLINE{ 42, 26, 28 }, WOOD_L{ 224, 180, 118 }, WOOD{ 156, 104, 62 }, WOOD_D{ 114, 72, 50 };
void RoundScene::drawBoardFrame(Frame& f, int ) {
    int bx = boardX(), by = boardY();
    uint8_t ol = C(OUTLINE), wl = C(WOOD_L), w = C(WOOD), wd = C(WOOD_D);
    f.rect(bx - 7, by - 7, 94, 174, ol);
    f.rect(bx - 6, by - 6, 92, 172, wl);
    f.rect(bx - 4, by - 4, 88, 168, w);
    f.rect(bx - 2, by - 2, 84, 164, wd);
    f.rect(bx - 1, by - 1, 82, 162, ol);
    for (int x = bx + 6; x < bx + 76; x += 11) { f.hline(x, x + 4, by - 4, wd); f.hline(x + 3, x + 7, by + 162, wd); }
    for (int y = by + 8; y < by + 156; y += 13) { f.vline(bx - 4, y, y + 5, wd); f.vline(bx + 83, y + 4, y + 9, wd); }
    int cx[2] = { bx - 7, bx + 80 }, cy[2] = { by - 7, by + 160 };
    for (int i = 0; i < 2; ++i) for (int j = 0; j < 2; ++j) {
        f.rect(cx[i], cy[j], 7, 7, ol);
        f.rect(cx[i] + 2, cy[j] + 2, 3, 3, C(194, 142, 88));
        f.set(cx[i] + 3, cy[j] + 3, C(214, 206, 190));
    }
    uint8_t cell = C(22, 26, 42), line = C(32, 38, 58), shade = C(14, 16, 28);
    f.rect(bx, by, 80, 160, cell);
    for (int x = 8; x < 80; x += 8) f.vline(bx + x, by, by + 159, line);
    for (int y = 8; y < 160; y += 8) f.hline(bx, bx + 79, by + y, line);
    f.hline(bx, bx + 79, by, shade); f.vline(bx, by, by + 159, shade);
}
void RoundScene::drawSign(Frame& f, int x, int y, int w, const char* label) {
    uint8_t ol = C(OUTLINE), post = C(84, 52, 40), shadow = C(60, 40, 36);
    f.rect(x + 6, y + 15, 2, 7, post); f.rect(x + w - 8, y + 15, 2, 7, post);
    f.rect(x + 1, y + 1, w, 16, shadow);
    f.rect(x, y, w, 16, ol);
    f.rect(x + 1, y + 1, w - 2, 14, C(WOOD));
    f.hline(x + 1, x + w - 2, y + 1, C(WOOD_L));
    f.hline(x + 1, x + w - 2, y + 14, C(WOOD_D));
    f.set(x + 2, y + 2, C(214, 206, 190)); f.set(x + w - 3, y + 2, C(214, 206, 190));
    f.hline(x + 1, x + 3, y + 8, C(WOOD_D)); f.hline(x + w - 6, x + w - 2, y + 6, C(WOOD_D));
    Font::center(f, label, x + w / 2, y + 5, C(250, 236, 200), ol);
}
void RoundScene::drawMemoryBook(Frame& f, int x, int y) {
    uint8_t ol = C(OUTLINE), red = C(150, 58, 70), redD = C(118, 40, 54), gold = C(222, 184, 92), page = C(240, 230, 208);
    f.rect(x + 3, y + 3, 56, 36, C(64, 38, 32));
    f.rect(x + 41, y - 7, 4, 10, gold); f.set(x + 41, y - 7, C(200, 160, 70));
    f.rect(x, y, 56, 36, ol);
    f.rect(x + 1, y + 1, 7, 34, redD);
    f.vline(x + 8, y + 1, y + 34, ol);
    f.rect(x + 9, y + 1, 46, 34, red);
    f.hline(x + 9, x + 54, y + 1, C(176, 80, 92));
    f.rect(x + 50, y + 2, 4, 32, page);
    for (int j = y + 4; j < y + 34; j += 3) f.hline(x + 50, x + 53, j, C(206, 196, 176));
    f.rect(x + 11, y + 3, 38, 30, gold); f.rect(x + 12, y + 4, 36, 28, red);
    uint8_t pink = C(240, 160, 176);
    const char* heart[6] = { ".##.##.", "#######", "#######", ".#####.", "..###..", "...#..." };
    for (int j = 0; j < 6; ++j) for (int i = 0; i < 7; ++i) if (heart[j][i] == '#') f.set(x + 27 + i, y + 7 + j, pink);
    f.set(x + 28, y + 8, C(250, 214, 222));
    Font::center(f, "MEMORY", x + 30, y + 16, C(252, 228, 152));
    Font::center(f, "BOOK", x + 30, y + 24, C(252, 228, 152));
}
void RoundScene::drawCommonUI(Frame& f) {
    drawSign(f, 8, 6, 44, "MENU");
    drawSign(f, 58, 6, 54, "SETTING");
    drawMemoryBook(f, 6, 226);
}
void RoundScene::flyToBook(Frame& f, int m, int sx, int sy, ItemDraw item, const char* label, int t) {
    if (m < 0 || m >= 26) return;
    uint8_t ol = C(OUTLINE), glow = C(255, 250, 200), gold = C(250, 222, 110);
    auto star = [&](int x, int y) { f.set(x, y, glow); f.set(x - 1, y, gold); f.set(x + 1, y, gold); f.set(x, y - 1, gold); f.set(x, y + 1, gold); };
    if (m < 10) for (int k = 0; k < 8; ++k) { float a = k * 0.785f + m * 0.3f, r = 10.0f + m * 2; star(sx + static_cast<int>(lround(cos(a) * r)), sy + static_cast<int>(lround(sin(a) * r * 0.6f))); }
    float u = max(0.0f, min(1.0f, (m - 4) / 10.0f));
    int ix = sx + static_cast<int>(lround((34 - sx) * u)), iy = sy + static_cast<int>(lround((236 - sy) * u)) - static_cast<int>(lround(sin(u * 3.14159f) * 60));
    int bounce = (m >= 14 && m < 18) ? (m % 2 ? 3 : 1) : 0;
    if (m >= 14) drawMemoryBook(f, 6, 226 - bounce);
    if (m < 15) { item(f, ix, iy - (m < 4 ? m : 4), t); if (m >= 4) { star(ix - 10, iy + 2); star(ix + 10, iy - 4); } }
    if (m >= 14 && m < 24) {
        int r = (m - 14) * 3 + 6;
        for (int k = 0; k < 8; ++k) { float a = k * 0.785f; star(34 + static_cast<int>(lround(cos(a) * r)), 242 + static_cast<int>(lround(sin(a) * r * 0.7f))); }
        if (m < 20) Font::center(f, label, 40, 212 - (m - 14), C(252, 228, 152), ol);
    }
}
void RoundScene::trackPills(const PlayStateReader& g, int t) {
    if (t == m_lastT) return;
    if (t < m_lastT) { m_flights.clear(); m_counts[0] = m_counts[1] = -1; }
    m_lastT = t;
    vector<array<int, 3>> now;
    for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x) if (g.at(x, y) >= 0 && g.pillAt(x, y) >= 0) now.push_back({x, y, g.pillAt(x, y)});
    for (int k = 0; k < 2; ++k) {
        int c = g.pillCount(k);
        if (m_counts[k] >= 0 && c > m_counts[k]) {
            int sx = cellX(4, t), sy = cellY(10, t);
            bool found = false;
            for (int row : g.clearingRows()) for (const auto& p : now) if (!found && p[1] == row && p[2] == k) { sx = cellX(p[0], t); sy = cellY(p[1], t); found = true; }
            for (const auto& p : m_pills) {
                if (found || p[2] != k) continue;
                bool still = false;
                for (const auto& q : now) if (q == p) still = true;
                if (!still) { sx = cellX(p[0], t); sy = cellY(p[1], t); found = true; }
            }
            for (int n = m_counts[k]; n < c; ++n) m_flights.push_back({k, sx, sy, t + (n - m_counts[k]) * 3});
        }
        m_counts[k] = c;
    }
    m_pills = now;
    for (size_t i = 0; i < m_flights.size();) {
        if (t - m_flights[i].t0 > 14) m_flights.erase(m_flights.begin() + static_cast<long>(i));
        else ++i;
    }
}
void RoundScene::drawPillFlights(Frame& f, int t) {
    const int fly = 10;
    uint8_t ol = C(42, 26, 28), white = C(250, 248, 240), spark = C(255, 240, 170), shine = C(255, 255, 255);
    for (const auto& p : m_flights) {
        int m = t - p.t0;
        if (m < 0) continue;
        int ex = pillX() + 12, ey = pillY() + 9 + p.kind * 26;
        if (m < fly) {
            auto at = [&](float u, int& x, int& y) {
                u = u * u * (3 - 2 * u);
                float cx = (p.sx + ex) / 2.0f, cy = static_cast<float>(min(p.sy, ey) - 60);
                x = static_cast<int>((1 - u) * (1 - u) * p.sx + 2 * (1 - u) * u * cx + u * u * ex + 0.5f);
                y = static_cast<int>((1 - u) * (1 - u) * p.sy + 2 * (1 - u) * u * cy + u * u * ey + 0.5f);
            };
            int x, y;
            for (int k = 1; k <= 3; ++k) {
                float u = (m - k * 0.6f) / (fly - 1);
                if (u < 0) continue;
                at(u, x, y);
                f.set(x, y, k % 2 ? spark : shine);
            }
            at(static_cast<float>(m) / (fly - 1), x, y);
            uint8_t body = p.kind == 0 ? C(70, 130, 230) : C(226, 70, 70), lit = p.kind == 0 ? C(170, 210, 255) : C(255, 170, 160);
            f.disc(x - 3, y, 3, ol); f.disc(x + 3, y, 3, ol); f.rect(x - 3, y - 3, 7, 7, ol);
            f.disc(x - 3, y, 2, body); f.rect(x - 3, y - 2, 4, 5, body);
            f.disc(x + 3, y, 2, white); f.rect(x + 1, y - 2, 3, 5, white);
            f.hline(x - 4, x - 2, y - 1, lit);
            if (m < 2) for (int a = 0; a < 8; ++a) {
                static const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1}, dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                f.set(p.sx + dx[a] * (5 + 2 * m), p.sy + dy[a] * (5 + 2 * m), shine);
            }
        } else if (m < fly + 4) {
            int k = m - fly, x0 = pillX() - 1 - k, y0 = pillY() - 1 + p.kind * 26 - k, w = 36 + 2 * k, h = 20 + 2 * k;
            uint8_t c = k % 2 == 0 ? shine : spark;
            f.hline(x0, x0 + w - 1, y0, c); f.hline(x0, x0 + w - 1, y0 + h - 1, c);
            f.vline(x0, y0, y0 + h - 1, c); f.vline(x0 + w - 1, y0, y0 + h - 1, c);
        }
    }
}
