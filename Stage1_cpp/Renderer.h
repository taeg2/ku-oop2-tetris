// Renderer.h - turns a Frame into Sixel output. Sends only the horizontal strips that changed since the
// last frame (Windows Terminal draws Sixel on a 10x20 px cell grid, so strips are cut on cell-row lines).
#pragma once
#include "Art.h"
#include "SixelEncoder.h"
#include <string>
#include <vector>

class Renderer {
public:
    void setScale(int s) { if (s != scale_) { scale_ = s; prev_.clear(); } }
    int scale() const { return scale_; }
    void forceFullRedraw() { prev_.clear(); }

    // returns the escape sequences to write this frame
    std::string present(const Frame& f) {
        int stripRows = stripHeight();                 // game rows per strip
        std::vector<uint32_t> now(f.px.size());
        const auto& pal = palette();
        for (size_t i = 0; i < f.px.size(); ++i) { const RGB& c = pal[f.px[i]]; now[i] = (c.r << 16) | (c.g << 8) | c.b; }
        std::string out;
        bool full = prev_.size() != now.size();
        for (int y0 = 0; y0 < Frame::H; y0 += stripRows) {
            int y1 = std::min(Frame::H, y0 + stripRows);
            bool changed = full;
            for (int i = y0 * Frame::W; !changed && i < y1 * Frame::W; ++i) changed = now[i] != prev_[i];
            if (!changed) continue;
            int cellRow = y0 * scale_ / 20 + 1;                    // 1-based terminal row
            out += "\x1b[" + std::to_string(cellRow) + ";1H";
            out += encoder_.encode(f, scale_, y0, y1);
        }
        prev_.swap(now);
        lastBytes_ = out.size();
        return out;
    }
    size_t lastBytes() const { return lastBytes_; }

private:
    int scale_ = 3;
    std::vector<uint32_t> prev_;
    SixelEncoder encoder_;
    size_t lastBytes_ = 0;

    int stripHeight() const {                       // smallest strip that starts on a 20 px cell row
        int l = 20; while (l % scale_) l += 20;
        return l / scale_;
    }
};
