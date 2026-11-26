#pragma once
#include <cstdint>
#include <iostream>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <initializer_list>
#include <utility>
#include <algorithm>
using namespace std;

class RGB { public: int r, g, b; };
inline vector<RGB>& palette() { static vector<RGB> p; return p; }
inline uint8_t C(int r, int g, int b) {
    auto& p = palette();
    for (size_t i = 0; i < p.size(); ++i)
        if (p[i].r == r && p[i].g == g && p[i].b == b) return static_cast<uint8_t>(i);
    if (p.size() >= 256) { cerr << "palette full!\n"; return 0; }
    p.push_back({ r, g, b });
    return static_cast<uint8_t>(p.size() - 1);
}
inline int& paletteEpoch() { static int e = 0; return e; }
inline void resetPalette() { palette().clear(); ++paletteEpoch(); }
inline uint8_t C(RGB c) { return C(c.r, c.g, c.b); }
inline RGB mix(RGB a, RGB b, float t) {
    return { static_cast<int>(a.r + (b.r - a.r) * t), static_cast<int>(a.g + (b.g - a.g) * t), static_cast<int>(a.b + (b.b - a.b) * t) };
}
class Frame {
public:
    using Pts = vector<pair<int,int>>;
    inline static const int W = 480, H = 270;
    vector<uint8_t> px;
    Frame() : px(W * H, 0) {}
    void set(int x, int y, uint8_t c) {
        if (static_cast<unsigned>(x) < static_cast<unsigned>(W) && static_cast<unsigned>(y) < static_cast<unsigned>(H)) px[y * W + x] = c;
    }
    uint8_t get(int x, int y) const {
        if (static_cast<unsigned>(x) < static_cast<unsigned>(W) && static_cast<unsigned>(y) < static_cast<unsigned>(H)) return px[y * W + x];
        return 0;
    }
    void rect(int x, int y, int w, int h, uint8_t c) {
        for (int j = y; j < y + h; ++j) for (int i = x; i < x + w; ++i) set(i, j, c);
    }
    void hline(int x0, int x1, int y, uint8_t c) { for (int x = x0; x <= x1; ++x) set(x, y, c); }
    void vline(int x, int y0, int y1, uint8_t c) { for (int y = y0; y <= y1; ++y) set(x, y, c); }
    void line(int x0, int y0, int x1, int y1, uint8_t c) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1, err = dx + dy;
        for (;;) {
            set(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
    void ellipse(int cx, int cy, int rx, int ry, uint8_t c) {
        for (int y = -ry; y <= ry; ++y)
            for (int x = -rx; x <= rx; ++x)
                if ((x * x) * (ry * ry) + (y * y) * (rx * rx) <= (rx * rx) * (ry * ry)) set(cx + x, cy + y, c);
    }
    void disc(int cx, int cy, int r, uint8_t c) { ellipse(cx, cy, r, r, c); }
    void dither(int x, int y, int w, int h, uint8_t a, uint8_t b) {
        for (int j = y; j < y + h; ++j) for (int i = x; i < x + w; ++i) set(i, j, ((i + j) & 1) ? a : b);
    }
    void poly(const vector<pair<int,int>>& p, uint8_t c) {
        if (p.size() < 3) return;
        int y0 = H, y1 = 0;
        for (auto& q : p) { y0 = min(y0, q.second); y1 = max(y1, q.second); }
        for (int y = max(0, y0); y <= min(H - 1, y1); ++y) {
            vector<int> xs;
            for (size_t i = 0; i < p.size(); ++i) {
                auto a = p[i], b = p[(i + 1) % p.size()];
                if ((a.second <= y && b.second > y) || (b.second <= y && a.second > y))
                    xs.push_back(a.first + (y - a.second) * (b.first - a.first) / (b.second - a.second));
            }
            sort(xs.begin(), xs.end());
            for (size_t i = 0; i + 1 < xs.size(); i += 2) hline(xs[i], xs[i + 1], y, c);
        }
    }
    void taper(int x0, int y0, int x1, int y1, float w0, float w1, uint8_t c) {
        int n = max(abs(x1 - x0), abs(y1 - y0)) + 1;
        for (int i = 0; i <= n; ++i) {
            float t = static_cast<float>(i) / n, w = w0 + (w1 - w0) * t;
            int cx = static_cast<int>(lround(x0 + (x1 - x0) * t)), cy = static_cast<int>(lround(y0 + (y1 - y0) * t));
            int r = static_cast<int>(w / 2);
            for (int dy = -r; dy <= r; ++dy) for (int dx = -r; dx <= r; ++dx)
                if (dx * dx + dy * dy <= r * r + r) set(cx + dx, cy + dy, c);
        }
    }
    void ditherEllipse(int cx, int cy, int rx, int ry, uint8_t c) {
        for (int y = -ry; y <= ry; ++y) for (int x = -rx; x <= rx; ++x) {
            long e = static_cast<long>(x) * x * ry * ry + static_cast<long>(y) * y * rx * rx, R = static_cast<long>(rx) * rx * ry * ry;
            if (e <= R * 7 / 10 || (e <= R && ((cx + x + cy + y) & 1))) set(cx + x, cy + y, c);
        }
    }
    void gradient(int x, int y, int w, int h, const vector<RGB>& ramp) {
        int n = static_cast<int>(ramp.size());
        for (int j = 0; j < h; ++j) {
            float t = static_cast<float>(j) / h * n;
            int k = static_cast<int>(t); if (k >= n) k = n - 1;
            float f = t - k;
            for (int i = 0; i < w; ++i) {
                int kk = k;
                if (f > 0.85f && k + 1 < n && ((i + j) & 1)) kk = k + 1;
                set(x + i, y + j, C(ramp[kk]));
            }
        }
    }
};
class Sprite {
public:
    vector<string> rows;
    int w = 0, h = 0;
    RGB key[128];
    bool has[128];
    const char* name;
    Sprite(const char* nm, initializer_list<const char*> r, initializer_list<pair<char, RGB>> keys) : Sprite(nm, vector<string>(r.begin(), r.end()), vector<pair<char, RGB>>(keys.begin(), keys.end())) {}
    Sprite(const char* nm, const vector<string>& r, const vector<pair<char, RGB>>& keys) : name(nm) {
        rows = r;
        h = static_cast<int>(rows.size());
        w = h ? static_cast<int>(rows[0].size()) : 0;
        for (int i = 0; i < 128; ++i) has[i] = false;
        for (auto& k : keys) { this->key[static_cast<int>(k.first) & 127] = k.second; has[static_cast<int>(k.first) & 127] = true; }
        for (int i = 0; i < h; ++i)
            if (static_cast<int>(rows[i].size()) != w)
                cerr << "[sprite " << nm << "] row " << i << " has " << rows[i].size() << " chars, expected " << w << "\n";
        for (auto& s : rows) for (char ch : s)
            if (ch != '.' && !has[static_cast<int>(ch) & 127])
                cerr << "[sprite " << nm << "] char '" << ch << "' has no color\n";
    }
};
inline void blit(Frame& f, const Sprite& sp, int x, int y, int s = 1, bool flip = false, int rowsMax = -1) {
    int hh = (rowsMax < 0 || rowsMax > sp.h) ? sp.h : rowsMax;
    int lut[128];
    for (int i = 0; i < 128; ++i) lut[i] = sp.has[i] ? C(sp.key[i]) : -1;
    for (int r = 0; r < hh; ++r)
        for (int c = 0; c < sp.w; ++c) {
            char ch = sp.rows[r][flip ? sp.w - 1 - c : c];
            if (ch == '.') continue;
            int col = lut[static_cast<int>(ch) & 127];
            if (col < 0) continue;
            for (int j = 0; j < s; ++j) for (int i = 0; i < s; ++i) f.set(x + c * s + i, y + r * s + j, static_cast<uint8_t>(col));
        }
}
inline const char* glyph(char ch) {
    switch (ch) {
    case 'A': return ".#." "#.#" "###" "#.#" "#.#";
    case 'C': return "###" "#.." "#.." "#.." "###";
    case 'D': return "##." "#.#" "#.#" "#.#" "##.";
    case 'E': return "###" "#.." "##." "#.." "###";
    case 'F': return "###" "#.." "##." "#.." "#..";
    case 'G': return "###" "#.." "#.#" "#.#" "###";
    case 'H': return "#.#" "#.#" "###" "#.#" "#.#";
    case 'I': return "###" ".#." ".#." ".#." "###";
    case 'K': return "#.#" "#.#" "##." "#.#" "#.#";
    case 'L': return "#.." "#.." "#.." "#.." "###";
    case 'M': return "#.#" "###" "###" "#.#" "#.#";
    case 'N': return "#.#" "##." "###" ".##" "#.#";
    case 'X': return "#.#" ".#." "#.#" ".#." "#.#";
    case 'O': return "###" "#.#" "#.#" "#.#" "###";
    case 'R': return "##." "#.#" "##." "#.#" "#.#";
    case 'S': return "###" "#.." "###" "..#" "###";
    case 'T': return "###" ".#." ".#." ".#." ".#.";
    case 'U': return "#.#" "#.#" "#.#" "#.#" "###";
    case 'Y': return "#.#" "#.#" ".#." ".#." ".#.";
    case '0': return "###" "#.#" "#.#" "#.#" "###";
    case '1': return ".#." "##." ".#." ".#." "###";
    case '2': return "###" "..#" "###" "#.." "###";
    case '3': return "###" "..#" ".##" "..#" "###";
    case '4': return "#.#" "#.#" "###" "..#" "..#";
    case '5': return "###" "#.." "###" "..#" "###";
    case '6': return "###" "#.." "###" "#.#" "###";
    case '7': return "###" "..#" "..#" ".#." ".#.";
    case '8': return "###" "#.#" "###" "#.#" "###";
    case '9': return "###" "#.#" "###" "..#" "###";
    case '/': return "..#" "..#" ".#." "#.." "#..";
    default: return "...............";
    }
}
inline void text(Frame& f, const string& s, int x, int y, uint8_t c, int scale = 1) {
    for (size_t k = 0; k < s.size(); ++k) {
        const char* g = glyph(s[k]);
        int len = static_cast<int>(strlen(g));
        for (int i = 0; i < 15 && i < len; ++i)
            if (g[i] == '#')
                f.rect(x + static_cast<int>(k) * 4 * scale + (i % 3) * scale, y + (i / 3) * scale, scale, scale, c);
    }
}
