#include "ui/MemoryBookView.h"
#include "ui/AlbumScreen.h"
#include "ui/MenuArt.h"
#include "ui/PixelFont.h"
#include <algorithm>
#include <cmath>
static const char* STAGE_NAMES[3] = { "CHILDHOOD", "SCHOOL DAYS", "ADULT LIFE" };
static void photo(Frame& f, int slot, int x, int y, int state, int t) {
    static Frame tmp;
    uint8_t ol = C(MenuArt::OUT), key = C(255, 0, 255);
    f.rect(x + 2, y + 2, 60, 44, C(150, 128, 100));
    f.rect(x, y, 60, 44, ol); f.rect(x + 1, y + 1, 58, 42, C(252, 250, 244));
    if (state == 2) {
        AlbumScreen::drawIcon(f, slot, x + 30, y + 22);
        uint8_t tape = C(240, 226, 160);
        f.rect(x - 3, y - 2, 12, 5, tape); f.rect(x + 51, y - 2, 12, 5, tape);
        return;
    }
    if (state == 1) {
        tmp.rect(0, 0, 60, 44, key);
        AlbumScreen::drawIcon(tmp, slot, 30, 22);
        const auto& p = palette();
        std::vector<uint8_t> gray(p.size(), 0);
        for (std::size_t i = 0; i < p.size() && i < 256; ++i) { int l = (p[i].r * 3 + p[i].g * 6 + p[i].b) / 10; gray[i] = C(std::min(255, l / 2 + 120) & ~15, std::min(255, l / 2 + 116) & ~15, std::min(255, l / 2 + 104) & ~15); }
        for (int j = 4; j < 40; ++j) for (int i = 4; i < 56; ++i) { uint8_t c = tmp.get(i, j); if (c != key && c < gray.size()) f.set(x + i, y + j, gray[c]); }
        bool on = (t / 4) % 2;
        uint8_t red = on ? C(230, 80, 80) : C(250, 160, 140);
        for (int i = 0; i < 60; i += 4) { f.hline(x + i, std::min(x + 59, x + i + 1), y - 2, red); f.hline(x + i, std::min(x + 59, x + i + 1), y + 45, red); }
        for (int j = 0; j < 44; j += 4) { f.vline(x - 2, y + j, std::min(y + 43, y + j + 1), red); f.vline(x + 61, y + j, std::min(y + 43, y + j + 1), red); }
        f.rect(x + 38, y - 7, 24, 11, ol); f.rect(x + 39, y - 6, 22, 9, C(230, 80, 80)); Font::draw(f, "NOW", x + 41, y - 5, C(250, 250, 244));
        return;
    }
    f.rect(x + 4, y + 4, 52, 36, C(214, 206, 192));
    for (int j = y + 6; j < y + 40; j += 3) f.hline(x + 6, x + 53, j, C(204, 196, 182));
    Font::center(f, "?", x + 30, y + 18, C(160, 150, 140));
}
MemoryBookView::MemoryBookView(Point origin) : View(origin) {}
void MemoryBookView::open(const std::vector<bool>& remembered, int current, const Frame& background) {
    m_open = true; m_openT = m_t; m_current = current;
    for (int i = 0; i < kSlots; ++i) m_remembered[i] = i < static_cast<int>(remembered.size()) && remembered[i];
    m_sel = current >= 0 ? current : 0;
    m_background = MenuArt::capture(background);
}
int MemoryBookView::rememberedCount() const { return static_cast<int>(std::count(m_remembered.begin(), m_remembered.end(), true)); }
void MemoryBookView::handle(Command command) {
    if (!m_open) return;
    if (command == Command::MoveLeft) m_sel = (m_sel + kSlots - 1) % kSlots;
    else if (command == Command::MoveRight) m_sel = (m_sel + 1) % kSlots;
    else if (command == Command::Rotate) m_sel = (m_sel + kSlots - 4) % kSlots;
    else if (command == Command::SoftDrop) m_sel = (m_sel + 4) % kSlots;
    else if (command == Command::Pause || command == Command::Confirm) m_open = false;
}
void MemoryBookView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) { m_accMs -= kFrameMs; ++m_t; }
}
void MemoryBookView::render() const {
    resetPalette();
    Frame& f = m_frame;
    MenuArt::backdrop(f, m_background);
    uint8_t ol = C(MenuArt::OUT), ink = C(MenuArt::INK), soft = C(150, 120, 100), cover = C(150, 46, 58), coverD = C(110, 32, 44);
    int a = m_t - m_openT, rise = std::max(0, 30 - a * 8);
    int bx = 22, by = 8 + rise, bw = 436, bh = 248;
    f.rect(bx + 4, by + 5, bw, bh, C(16, 10, 12));
    f.rect(bx, by, bw, bh, ol); f.rect(bx + 1, by + 1, bw - 2, bh - 2, cover); f.rect(bx + 1, by + bh - 6, bw - 2, 5, coverD);
    int px = bx + 10, py = by + 6, pw = bw - 18, ph = bh - 14;
    f.rect(px, py, pw, ph, ol); f.rect(px + 1, py + 1, pw - 2, ph - 2, C(MenuArt::PAPER));
    for (int y = py + 34; y < py + ph - 4; y += 12) f.hline(px + 30, px + pw - 6, y, C(232, 220, 196));
    f.vline(px + 26, py + 2, py + ph - 3, C(236, 170, 170));
    for (int k = 0; k < 7; ++k) { int ry = py + 14 + k * 32; f.disc(px + 12, ry, 5, ol); f.disc(px + 12, ry, 4, C(200, 200, 208)); f.disc(px + 12, ry, 2, C(MenuArt::PAPER)); f.hline(bx - 2, px + 9, ry, C(170, 170, 180)); }
    int got = rememberedCount();
    Font::draw(f, "MEMORY BOOK", px + 36, py + 10, ink);
    std::string count = std::to_string(got) + " / 12 MEMORIES";
    Font::draw(f, count, px + pw - 14 - Font::width(count), py + 10, ink);
    for (int i = 0; i < kSlots; ++i) { int hx = px + 150 + i * 11, hy = py + 13; uint8_t c = m_remembered[i] ? C(230, 80, 100) : i == m_current ? ((m_t / 4) % 2 ? C(240, 170, 170) : C(214, 206, 192)) : C(214, 206, 192); f.disc(hx - 1, hy - 1, 2, c); f.disc(hx + 2, hy - 1, 2, c); f.poly({ {hx - 3, hy}, {hx + 4, hy}, {hx + 1, hy + 4} }, c); }
    f.hline(px + 34, px + pw - 10, py + 24, soft);
    for (int s = 0; s < 3; ++s) {
        int ry = py + 36 + s * 62, have = 0;
        for (int k = 0; k < 4; ++k) if (m_remembered[s * 4 + k]) ++have;
        Font::draw(f, std::string("STAGE ") + char('1' + s), px + 36, ry + 8, soft);
        Font::draw(f, STAGE_NAMES[s], px + 36, ry + 18, ink);
        std::string part = std::to_string(have) + " / 4";
        Font::draw(f, part, px + 36, ry + 30, have == 4 ? C(80, 150, 80) : soft);
        if (have == 4) { int sx = px + 36 + Font::width(part) + 6, sy = ry + 33; f.poly({ {sx, sy - 4}, {sx + 1, sy - 1}, {sx + 4, sy}, {sx + 1, sy + 1}, {sx, sy + 4}, {sx - 1, sy + 1}, {sx - 4, sy}, {sx - 1, sy - 1} }, C(240, 200, 80)); }
        for (int k = 0; k < 4; ++k) {
            int slot = s * 4 + k, cx = px + 128 + k * 72, cy = ry;
            int state = m_remembered[slot] ? 2 : slot == m_current ? 1 : 0;
            photo(f, slot, cx, cy, state, m_t);
            std::string cap = state == 2 ? AlbumScreen::memoryName(slot) : state == 1 ? AlbumScreen::roundName(slot) : "???";
            Font::center(f, cap, cx + 30, cy + 49, state == 0 ? soft : ink);
            if (slot == m_sel) { int bob = (m_t / 3) % 2; uint8_t y = C(240, 150, 60); for (int i = 0; i < 2; ++i) { f.hline(cx - 5 - i, cx + 65 + i, cy - 5 - i - bob, y); f.hline(cx - 5 - i, cx + 65 + i, cy + 57 + i + bob, y); f.vline(cx - 5 - i, cy - 5 - i - bob, cy + 57 + i + bob, y); f.vline(cx + 65 + i, cy - 5 - i - bob, cy + 57 + i + bob, y); } }
        }
    }
    std::string detail;
    int s = m_sel / 4, k = m_sel % 4;
    std::string where = std::string("STAGE ") + char('1' + s) + " ROUND " + char('1' + k) + "  ";
    if (m_remembered[m_sel]) detail = where + AlbumScreen::roundName(m_sel) + " - " + AlbumScreen::memoryName(m_sel);
    else if (m_sel == m_current) detail = where + "NOW PLAYING: " + AlbumScreen::roundName(m_sel);
    else detail = where + "NOT REMEMBERED YET";
    if (got == kSlots) detail = "EVERY MEMORY IS BACK.";
    MenuArt::hint(f, detail + "    ESC: CLOSE");
}
void MemoryBookView::draw(Console& console) const {
    if (m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
