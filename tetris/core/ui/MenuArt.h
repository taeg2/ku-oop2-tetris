#pragma once
#include "ui/PixelArt.h"
#include "ui/PixelFont.h"
#include <string>
#include <vector>
// 메뉴, 설정, 기억의 책 화면이 같이 쓰는 그림. 멈춘 게임 화면을 어둡게 깔고 그 위에 나무판을 그린다
namespace MenuArt {
inline const RGB OUT{ 42, 26, 28 }, WOOD{ 156, 104, 62 }, WOOD_L{ 224, 180, 118 }, WOOD_D{ 114, 72, 50 }, PAPER{ 250, 240, 214 }, INK{ 70, 48, 40 };
inline std::vector<RGB> capture(const Frame& f) {
    std::vector<RGB> out(f.px.size());
    const auto& p = palette();
    for (std::size_t i = 0; i < f.px.size(); ++i) out[i] = f.px[i] < p.size() ? p[f.px[i]] : RGB{ 0, 0, 0 };
    return out;
}
inline void backdrop(Frame& f, const std::vector<RGB>& bg) {
    if (bg.size() != f.px.size()) {
        f.gradient(0, 0, Frame::W, Frame::H, { {46, 30, 28}, {72, 48, 38} });
        for (int y = 18; y < Frame::H; y += 24) { f.hline(0, Frame::W - 1, y, C(36, 24, 22)); f.hline(0, Frame::W - 1, y + 1, C(84, 58, 46)); }
        for (int y = 0; y < Frame::H; y += 24) for (int x = (y / 24 % 2) * 60; x < Frame::W; x += 120) f.vline(x, y + 2, y + 17, C(36, 24, 22));
        return;
    }
    for (std::size_t i = 0; i < f.px.size(); ++i) { const RGB& c = bg[i]; int l = (c.r * 3 + c.g * 6 + c.b) / 10; f.px[i] = C(((c.r + l) / 5) & ~7, ((c.g + l) / 5) & ~7, (((c.b + l) / 5) + 8) & ~7); }
}
inline void board(Frame& f, int x, int y, int w, int h) {
    uint8_t ol = C(OUT), wl = C(WOOD_L), wd = C(WOOD_D), wood = C(WOOD);
    f.rect(x + 3, y + 4, w, h, C(16, 10, 12));
    f.rect(x, y, w, h, ol); f.rect(x + 1, y + 1, w - 2, h - 2, wl); f.rect(x + 3, y + 3, w - 6, h - 6, wood);
    for (int py = y + 18; py < y + h - 6; py += 18) { f.hline(x + 3, x + w - 4, py, wd); f.hline(x + 3, x + w - 4, py + 1, C(176, 122, 76)); }
    for (int k = 0; k < 9; ++k) { int gx = x + 12 + (k * 53) % (w - 24), gy = y + 8 + (k * 31) % (h - 16); f.hline(gx, gx + 6 + k % 4, gy, wd); }
    int cx[2] = { x + 6, x + w - 8 }, cy[2] = { y + 6, y + h - 8 };
    for (int i = 0; i < 2; ++i) for (int j = 0; j < 2; ++j) { f.rect(cx[i], cy[j], 3, 3, ol); f.set(cx[i] + 1, cy[j] + 1, C(214, 206, 190)); }
}
inline void plank(Frame& f, int x, int y, int w, int h, bool lit) {
    uint8_t ol = C(OUT), face = lit ? C(250, 222, 150) : C(WOOD_L), edge = lit ? C(214, 170, 96) : C(196, 146, 92), dark = C(WOOD_D);
    f.rect(x + 2, y + 3, w, h, C(30, 18, 18));
    f.rect(x, y, w, h, ol); f.rect(x + 1, y + 1, w - 2, h - 2, face); f.hline(x + 1, x + w - 2, y + h - 3, edge); f.hline(x + 1, x + w - 2, y + h - 2, dark);
    f.set(x + 4, y + 4, ol); f.set(x + w - 5, y + 4, ol);
}
inline void rope(Frame& f, int x, int y0, int y1) { for (int y = y0; y < y1; ++y) f.set(x + ((y / 2) % 2), y, (y / 2) % 2 ? C(196, 160, 110) : C(150, 116, 76)); }
inline void hint(Frame& f, const std::string& text) { int w = Font::width(text) + 14, y = Frame::H - 15; f.rect(Frame::W / 2 - w / 2, y, w, 13, C(20, 14, 16)); Font::center(f, text, Frame::W / 2, y + 3, C(230, 220, 200)); }
}
