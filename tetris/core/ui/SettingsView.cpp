#include "ui/SettingsView.h"
#include "ui/MenuArt.h"
#include "ui/PixelFont.h"
#include <algorithm>
using namespace std;

class KeyRow { public: const char* keys[3]; const char* action; };
static const KeyRow ROWS[8] = {
    { { "<", ">", "A D" }, "MOVE LEFT / RIGHT" },
    { { "^", "W", nullptr }, "ROTATE" },
    { { "v", "S", nullptr }, "SOFT DROP" },
    { { "SPACE", nullptr, nullptr }, "HARD DROP" },
    { { "1", "2", nullptr }, "BLUE / RED PILL" },
    { { "ENTER", nullptr, nullptr }, "OK / NEXT" },
    { { "ESC", nullptr, nullptr }, "MENU / BACK" },
    { { "Q", nullptr, nullptr }, "QUIT GAME" },
};
static int keycap(Frame& f, const string& k, int x, int y) {
    uint8_t ol = C(MenuArt::OUT), cap = C(236, 232, 222), side = C(170, 164, 156), ink = C(60, 52, 56);
    bool arrow = k == "<" || k == ">" || k == "^" || k == "v";
    int w = arrow ? 13 : Font::width(k) + 8;
    f.rect(x, y, w, 13, ol); f.rect(x + 1, y + 1, w - 2, 11, side); f.rect(x + 1, y + 1, w - 2, 9, cap);
    int cx = x + w / 2, cy = y + 5;
    if (k == "<") f.poly({ {cx - 3, cy}, {cx + 2, cy - 3}, {cx + 2, cy + 3} }, ink);
    else if (k == ">") f.poly({ {cx + 3, cy}, {cx - 2, cy - 3}, {cx - 2, cy + 3} }, ink);
    else if (k == "^") f.poly({ {cx, cy - 3}, {cx - 3, cy + 2}, {cx + 3, cy + 2} }, ink);
    else if (k == "v") f.poly({ {cx, cy + 3}, {cx - 3, cy - 2}, {cx + 3, cy - 2} }, ink);
    else Font::draw(f, k, x + 4, y + 2, ink);
    return w;
}
SettingsView::SettingsView(Point origin) : View(origin) {}
void SettingsView::open(bool soundOn, const Frame& background) { m_open = true; m_sound = soundOn; m_background = MenuArt::capture(background); }
void SettingsView::handle(Command command) {
    if (!m_open) return;
    if (command == Command::MoveLeft || command == Command::MoveRight || command == Command::Confirm || command == Command::HardDrop) { m_sound = !m_sound; m_flipT = m_t; }
    else if (command == Command::Pause) m_open = false;
}
void SettingsView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) { m_accMs -= kFrameMs; ++m_t; }
}
void SettingsView::render() const {
    resetPalette();
    Frame& f = m_frame;
    MenuArt::backdrop(f, m_background);
    int w = 300, h = 236, x = Frame::W / 2 - w / 2, y = 10, cx = Frame::W / 2;
    MenuArt::board(f, x, y, w, h);
    MenuArt::plank(f, cx - 60, y + 9, 120, 22, false);
    Font::center(f, "SETTINGS", cx, y + 16, C(MenuArt::INK));
    uint8_t ol = C(MenuArt::OUT), paper = C(MenuArt::PAPER), ink = C(MenuArt::INK);
    int sy = y + 40;
    MenuArt::plank(f, x + 20, sy, w - 40, 30, true);
    Font::draw(f, "SOUND", x + 34, sy + 11, ink);
    int swx = x + w - 104, swy = sy + 7, k = min(4, m_t - m_flipT), pos = m_sound ? (k * 5) : (20 - k * 5);
    f.rect(swx, swy, 42, 16, ol); f.rect(swx + 1, swy + 1, 40, 14, m_sound ? C(110, 180, 90) : C(150, 146, 140));
    Font::draw(f, m_sound ? "ON" : "OFF", m_sound ? swx + 6 : swx + 20, swy + 4, C(250, 250, 244));
    f.rect(swx + 1 + pos, swy + 1, 20, 14, ol); f.rect(swx + 2 + pos, swy + 2, 18, 12, C(246, 242, 232)); f.vline(swx + 10 + pos, swy + 5, swy + 10, C(200, 196, 188)); f.vline(swx + 12 + pos, swy + 5, swy + 10, C(200, 196, 188));
    int nx = swx + 50, ny = swy + 8;
    f.poly({ {nx, ny - 2}, {nx + 3, ny - 2}, {nx + 7, ny - 6}, {nx + 7, ny + 6}, {nx + 3, ny + 2}, {nx, ny + 2} }, ink);
    if (m_sound) { f.set(nx + 10, ny - 3, ink); f.set(nx + 11, ny - 1, ink); f.set(nx + 11, ny + 1, ink); f.set(nx + 10, ny + 3, ink); f.set(nx + 13, ny - 5, ink); f.set(nx + 14, ny - 2, ink); f.set(nx + 14, ny + 2, ink); f.set(nx + 13, ny + 5, ink); }
    else { f.line(nx + 9, ny - 3, nx + 14, ny + 3, C(200, 60, 60)); f.line(nx + 9, ny + 3, nx + 14, ny - 3, C(200, 60, 60)); }
    if ((m_t / 3) % 2) { f.poly({ {x + 12, sy + 9}, {x + 12, sy + 21}, {x + 18, sy + 15} }, C(250, 236, 160)); }
    int ty = sy + 40;
    f.rect(x + 20, ty, w - 40, h - (ty - y) - 12, ol); f.rect(x + 21, ty + 1, w - 42, h - (ty - y) - 14, paper);
    for (int ly = ty + 26; ly < y + h - 14; ly += 15) f.hline(x + 24, x + w - 25, ly + 12, C(226, 214, 190));
    Font::center(f, "CONTROLS", cx, ty + 6, ink);
    f.hline(cx - 30, cx + 30, ty + 15, C(196, 150, 100));
    for (int r = 0; r < 8; ++r) {
        int ry = ty + 22 + r * 15, kx = x + 34;
        for (int j = 0; j < 3 && ROWS[r].keys[j]; ++j) {
            string key = ROWS[r].keys[j];
            bool arrow = key == "<" || key == ">" || key == "^" || key == "v";
            if (j > 0 && !arrow) { string prev = ROWS[r].keys[j - 1]; if (prev == "<" || prev == ">" || prev == "^" || prev == "v") { Font::draw(f, "/", kx, ry + 3, ink); kx += 8; } }
            kx += keycap(f, key, kx, ry) + 3;
        }
        Font::draw(f, ROWS[r].action, x + 146, ry + 3, ink);
    }
    MenuArt::hint(f, "LEFT/RIGHT/ENTER: SOUND ON/OFF   ESC: BACK");
}
void SettingsView::draw(Console& console) const {
    if (m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
