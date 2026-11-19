#include "ui/RoundScene.h"
#include "ui/PixelFont.h"
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
