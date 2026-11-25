#include "ui/MenuView.h"
#include "ui/MenuArt.h"
#include "ui/PixelFont.h"
#include <algorithm>
#include <cmath>
static const char* LABELS[4] = { "RESUME", "MEMORY BOOK", "SETTINGS", "QUIT TO TITLE" };
static void icon(Frame& f, int k, int cx, int cy, int t) {
    uint8_t ol = C(MenuArt::OUT);
    if (k == 0) { f.poly({ {cx - 3, cy - 5}, {cx - 3, cy + 5}, {cx + 5, cy} }, ol); f.poly({ {cx - 2, cy - 3}, {cx - 2, cy + 3}, {cx + 3, cy} }, C(110, 180, 90)); }
    if (k == 1) { f.rect(cx - 6, cy - 5, 12, 11, ol); f.rect(cx - 5, cy - 4, 10, 9, C(170, 50, 64)); f.rect(cx - 5, cy - 4, 2, 9, C(130, 36, 50)); f.set(cx + 1, cy - 1, C(250, 200, 210)); f.set(cx + 2, cy - 1, C(250, 200, 210)); f.set(cx + 1, cy, C(250, 200, 210)); f.set(cx + 2, cy, C(250, 200, 210)); f.set(cx + 1, cy + 1, C(250, 200, 210)); }
    if (k == 2) { float a0 = t * 0.15f; for (int i = 0; i < 8; ++i) { float a = a0 + i * 0.785f; f.disc(cx + static_cast<int>(std::lround(std::cos(a) * 5)), cy + static_cast<int>(std::lround(std::sin(a) * 5)), 1, ol); } f.disc(cx, cy, 5, ol); f.disc(cx, cy, 4, C(150, 150, 160)); f.disc(cx, cy, 1, ol); }
    if (k == 3) { f.rect(cx - 5, cy - 6, 10, 13, ol); f.rect(cx - 4, cy - 5, 8, 11, C(130, 86, 54)); f.set(cx + 2, cy, C(230, 190, 90)); f.hline(cx - 6, cx + 6, cy + 7, ol); }
}
MenuView::MenuView(Point origin) : View(origin) {}
void MenuView::open(const Frame& background) { m_open = true; m_sel = 0; m_choice = Choice::None; m_openT = m_t; m_background = MenuArt::capture(background); }
void MenuView::clearChoice() { m_choice = Choice::None; }
void MenuView::handle(Command command) {
    if (!m_open) return;
    if (command == Command::Rotate) m_sel = (m_sel + kItems - 1) % kItems;
    else if (command == Command::SoftDrop) m_sel = (m_sel + 1) % kItems;
    else if (command == Command::Pause) { m_choice = Choice::Resume; m_open = false; }
    else if (command == Command::Confirm || command == Command::HardDrop) {
        static const Choice picks[4] = { Choice::Resume, Choice::MemoryBook, Choice::Settings, Choice::QuitToTitle };
        m_choice = picks[m_sel];
        if (m_choice == Choice::Resume || m_choice == Choice::QuitToTitle) m_open = false;
    }
}
void MenuView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) { m_accMs -= kFrameMs; ++m_t; }
}
void MenuView::render() const {
    resetPalette();
    Frame& f = m_frame;
    MenuArt::backdrop(f, m_background);
    int a = m_t - m_openT, drop = std::max(0, 40 - a * 10), cx = Frame::W / 2;
    int top = 22 - drop, w = 196, x = cx - w / 2;
    MenuArt::rope(f, x + 30, 0, top + 6); MenuArt::rope(f, x + w - 32, 0, top + 6);
    MenuArt::board(f, x, top, w, 214);
    MenuArt::plank(f, cx - 50, top + 10, 100, 22, false);
    Font::center(f, "MENU", cx, top + 17, C(MenuArt::INK));
    Font::center(f, "PAUSED", cx, top + 38, C(250, 230, 190));
    for (int k = 0; k < kItems; ++k) {
        bool sel = k == m_sel;
        int py = top + 54 + k * 36, px = x + 18 + (sel ? 4 : 0);
        MenuArt::plank(f, px, py, w - 36, 28, sel);
        icon(f, k, px + 18, py + 12, m_t);
        Font::draw(f, LABELS[k], px + 34, py + 9, C(MenuArt::INK));
        if (sel) { int bob = (m_t / 3) % 2; f.poly({ {px - 12 + bob, py + 7}, {px - 12 + bob, py + 19}, {px - 5 + bob, py + 13} }, C(250, 236, 160)); f.poly({ {px - 11 + bob, py + 9}, {px - 11 + bob, py + 17}, {px - 7 + bob, py + 13} }, C(230, 150, 60)); }
    }
    MenuArt::hint(f, "UP/DOWN: CHOOSE   ENTER: OK   ESC: BACK TO GAME");
}
void MenuView::draw(Console& console) const {
    if (m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
