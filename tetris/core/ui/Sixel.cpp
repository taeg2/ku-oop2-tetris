#include "ui/Sixel.h"
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

namespace {

int toPercent(std::uint8_t v) {
    return (static_cast<int>(v) * 100 + 127) / 255;
}

void appendRun(std::string& out, char ch, int count) {
    if (count <= 0) {
        return;
    }
    if (count >= 4) {
        out += "!" + std::to_string(count) + ch;
    } else {
        out.append(static_cast<std::size_t>(count), ch);
    }
}

}

namespace Sixel {

std::string encode(const PixelImage& image, int scale) {
    const int logicalW = image.width();
    const int logicalH = image.height();
    const int rasterW = logicalW * scale;
    const int rasterH = logicalH * scale;

    auto colorAt = [&](int rx, int ry) -> const Rgb* {
        const int lx = rx / scale;
        const int ly = ry / scale;
        if (image.isBackground(lx, ly)) {
            return nullptr;
        }
        static thread_local Rgb cache;
        cache = image.at(lx, ly);
        return &cache;
    };

    std::vector<Rgb> palette;
    std::map<std::uint32_t, int> paletteIndex;
    auto keyOf = [](Rgb c) { return (static_cast<std::uint32_t>(c.r) << 16) |
                                     (static_cast<std::uint32_t>(c.g) << 8) | c.b; };
    for (int ry = 0; ry < rasterH; ++ry) {
        for (int rx = 0; rx < rasterW; ++rx) {
            if (const Rgb* c = colorAt(rx, ry)) {
                const auto key = keyOf(*c);
                if (paletteIndex.find(key) == paletteIndex.end()) {
                    paletteIndex[key] = static_cast<int>(palette.size());
                    palette.push_back(*c);
                }
            }
        }
    }

    std::string out;
    out += "\x1bP0;1;0q";
    out += "\"1;1;" + std::to_string(rasterW) + ";" + std::to_string(rasterH);
    for (std::size_t i = 0; i < palette.size(); ++i) {
        const Rgb c = palette[i];
        out += "#" + std::to_string(i) + ";2;" + std::to_string(toPercent(c.r)) + ";" +
               std::to_string(toPercent(c.g)) + ";" + std::to_string(toPercent(c.b));
    }

    for (int bandY = 0; bandY < rasterH; bandY += 6) {
        const int bandHeight = std::min(6, rasterH - bandY);
        bool firstColorInBand = true;
        for (std::size_t colorIdx = 0; colorIdx < palette.size(); ++colorIdx) {
            std::string data;
            bool used = false;
            char runChar = 0;
            int runCount = 0;
            for (int rx = 0; rx < rasterW; ++rx) {
                int mask = 0;
                for (int dy = 0; dy < bandHeight; ++dy) {
                    const Rgb* c = colorAt(rx, bandY + dy);
                    if (c && keyOf(*c) == keyOf(palette[colorIdx])) {
                        mask |= (1 << dy);
                        used = true;
                    }
                }
                const char ch = static_cast<char>(0x3F + mask);
                if (ch == runChar) {
                    ++runCount;
                } else {
                    appendRun(data, runChar, runCount);
                    runChar = ch;
                    runCount = 1;
                }
            }
            appendRun(data, runChar, runCount);
            if (!used) {
                continue;
            }
            if (!firstColorInBand) {
                out += "$";
            }
            out += "#" + std::to_string(colorIdx) + data;
            firstColorInBand = false;
        }
        if (bandY + 6 < rasterH) {
            out += "-";
        }
    }

    out += "\x1b\\";
    return out;
}

}
