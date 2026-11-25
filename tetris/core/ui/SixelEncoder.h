#pragma once
#include "ui/PixelArt.h"
#include <string>
class SixelEncoder {
public:
    std::string encode(const Frame& f, int s) const { return encode(f, s, 0, Frame::H); }
    std::string encode(const Frame& f, int s, int y0, int y1) const {
        const int W = Frame::W * s, H = (y1 - y0) * s;
        std::string out;
        out.reserve(1 << 18);
        out += "\x1bP0;1;0q\"1;1;" + std::to_string(W) + ";" + std::to_string(H);
        const auto& pal = palette();
        bool inUse[256] = {};
        for (int y = y0; y < y1; ++y) for (int x = 0; x < Frame::W; ++x) inUse[f.px[y * Frame::W + x]] = true;
        for (size_t i = 0; i < pal.size(); ++i)
            if (inUse[i])
                out += "#" + std::to_string(i) + ";2;" + std::to_string(pal[i].r * 100 / 255) + ";" +
                       std::to_string(pal[i].g * 100 / 255) + ";" + std::to_string(pal[i].b * 100 / 255);
        for (int band = 0; band < H; band += 6) {
            const uint8_t* rows[6];
            for (int k = 0; k < 6; ++k) rows[k] = (band + k < H) ? &f.px[(y0 + (band + k) / s) * Frame::W] : nullptr;
            bool used[256] = {};
            for (int k = 0; k < 6; ++k) if (rows[k]) for (int x = 0; x < Frame::W; ++x) used[rows[k][x]] = true;
            bool first = true;
            for (int c = 0; c < 256; ++c) {
                if (!used[c]) continue;
                if (!first) out += '$';
                first = false;
                out += '#'; out += std::to_string(c);
                char prev = 0; int run = 0;
                for (int gx = 0; gx < Frame::W; ++gx) {
                    int bits = 0;
                    for (int k = 0; k < 6; ++k) if (rows[k] && rows[k][gx] == c) bits |= 1 << k;
                    char ch = static_cast<char>(63 + bits);
                    if (ch == prev) run += s;
                    else { emit(out, prev, run); prev = ch; run = s; }
                }
                if (prev != '?') emit(out, prev, run);
            }
            out += '-';
        }
        out += "\x1b\\";
        return out;
    }
private:
    static void emit(std::string& out, char ch, int run) {
        if (run <= 0) return;
        if (run > 3) { out += '!'; out += std::to_string(run); out += ch; }
        else out.append(run, ch);
    }
};
