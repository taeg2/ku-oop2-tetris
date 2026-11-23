#pragma once
#include "ui/PixelArt.h"
#include "ui/PixelFont.h"
namespace Stage2UI {
inline void notebookItem(Frame& f, int x, int y, int) {
    uint8_t ol = C(42, 26, 28), cover = C(70, 110, 190), coverD = C(50, 80, 150), page = C(252, 250, 240), line = C(170, 190, 220);
    f.rect(x - 8, y - 6, 17, 13, ol); f.rect(x - 7, y - 5, 15, 11, cover); f.rect(x - 5, y - 4, 12, 9, page);
    for (int k = 0; k < 3; ++k) f.hline(x - 3, x + 5, y - 2 + k * 3, line);
    for (int k = 0; k < 4; ++k) f.set(x - 7, y - 4 + k * 3, C(200, 200, 210));
    f.vline(x - 6, y - 5, y + 5, coverD);
}
inline void ballItem(Frame& f, int x, int y, int t) {
    uint8_t ol = C(42, 26, 28);
    f.disc(x, y, 7, ol); f.disc(x, y, 6, C(250, 250, 248)); f.disc(x - 2, y - 2, 2, C(255, 255, 255));
    int r = (t / 2) % 2;
    f.rect(x - 1 + r, y - 1, 3, 3, ol); f.set(x - 5, y - 2 + r, ol); f.set(x + 4, y + 2 - r, ol); f.set(x + 1 - r, y + 5, ol); f.set(x - 1 + r, y - 5, ol);
}
inline void capItem(Frame& f, int x, int y, int) {
    uint8_t ol = C(42, 26, 28), cap = C(34, 36, 50), capL = C(80, 82, 104), gold = C(232, 188, 72);
    f.rect(x - 5, y, 11, 5, ol); f.rect(x - 4, y, 9, 4, cap);
    f.poly({ {x - 11, y - 1}, {x, y - 6}, {x + 11, y - 1}, {x, y + 3} }, ol); f.poly({ {x - 9, y - 1}, {x, y - 5}, {x + 9, y - 1}, {x, y + 2} }, cap); f.line(x - 7, y - 2, x, y - 5, capL);
    f.set(x, y - 2, gold); f.line(x, y - 2, x + 7, y + 1, gold); f.vline(x + 7, y + 1, y + 5, gold);
}
inline void idItem(Frame& f, int x, int y, int) {
    uint8_t ol = C(42, 26, 28), strap = C(70, 120, 200);
    f.line(x - 6, y - 12, x - 1, y - 5, strap); f.line(x + 6, y - 12, x + 1, y - 5, strap);
    f.rect(x - 6, y - 5, 13, 15, ol); f.rect(x - 5, y - 4, 11, 13, C(250, 250, 244)); f.rect(x - 5, y - 4, 11, 3, strap);
    f.rect(x - 4, y, 4, 5, C(206, 222, 238)); f.set(x - 2, y + 1, C(168, 92, 44)); f.hline(x + 1, x + 4, y + 1, C(150, 150, 170)); f.hline(x + 1, x + 3, y + 3, C(150, 150, 170));
}

inline void pills(Frame& f, int t) {
    uint8_t ol = C(42, 26, 28);
    struct P { RGB a, b, lit; const char* key; };
    const P ps[2] = { { {70, 130, 230}, {40, 88, 186}, {170, 210, 255}, "1" }, { {226, 70, 70}, {168, 40, 48}, {255, 170, 160}, "2" } };
    for (int k = 0; k < 2; ++k) {
        int x = 28, y = 132 + k * 26;
        f.rect(x - 1, y - 1, 36, 20, ol); f.rect(x, y, 34, 18, C(244, 236, 214)); f.hline(x, x + 33, y, C(255, 250, 236));
        int cx = x + 4, cy = y + 5, bob = (t / 8 + k) % 2;
        f.disc(cx + 3, cy + 4 - bob, 4, ol); f.disc(cx + 13, cy + 4 - bob, 4, ol); f.rect(cx + 3, cy - bob, 10, 9, ol);
        f.disc(cx + 3, cy + 4 - bob, 3, C(ps[k].a)); f.rect(cx + 3, cy + 1 - bob, 5, 7, C(ps[k].a));
        f.disc(cx + 13, cy + 4 - bob, 3, C(250, 248, 240)); f.rect(cx + 8, cy + 1 - bob, 5, 7, C(250, 248, 240));
        f.hline(cx + 3, cx + 12, cy + 7 - bob, C(ps[k].b)); f.hline(cx + 8, cx + 13, cy + 7 - bob, C(206, 200, 190));
        f.hline(cx + 2, cx + 6, cy + 2 - bob, C(ps[k].lit)); f.vline(cx + 8, cy + 1 - bob, cy + 7 - bob, ol);
        f.rect(x + 22, y + 3, 10, 11, ol); f.rect(x + 23, y + 4, 8, 9, C(252, 250, 244)); f.hline(x + 23, x + 30, y + 12, C(200, 196, 184));
        Font::draw(f, ps[k].key, x + 25, y + 5, ol);
    }
}
}
