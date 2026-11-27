#include "ui/GameOverView.h"
#include "ui/PixelFont.h"
#include "ui/ToyBlocks.h"
using namespace std;
static void bigText(Frame& f, const string& s, int cx, int y, int scale, uint8_t color, uint8_t shadow) {
    int x0 = cx - (static_cast<int>(s.size()) * 6 - 1) * scale / 2;
    for (int pass = 0; pass < 2; ++pass) {
        for (size_t k = 0; k < s.size(); ++k) {
            const char* g = Font::glyph(s[k]);
            if (!g) continue;
            for (int i = 0; i < 25; ++i) {
                if (g[i] != '#') continue;
                f.rect(x0 + (static_cast<int>(k) * 6 + i % 5) * scale, y + (i / 5) * scale + (pass == 0 ? scale : 0), scale, scale, pass == 0 ? shadow : color);
            }
        }
    }
}
static void button(Frame& f, int cx, int y, int w, const char* label, bool focus, int t) {
    uint8_t ol = C(42, 26, 28), gold = C(250, 214, 92);
    int x = cx - w / 2;
    f.rect(x - 2, y - 2, w + 4, 26, focus && (t / 4) % 2 == 0 ? gold : ol);
    f.rect(x - 1, y - 1, w + 2, 24, ol);
    f.rect(x, y, w, 22, focus ? C(252, 246, 224) : C(70, 66, 88));
    f.hline(x, x + w - 1, y, focus ? C(255, 255, 244) : C(92, 88, 112));
    Font::center(f, label, cx, y + 8, focus ? ol : C(170, 166, 190));
}
GameOverView::GameOverView(Point origin) : View(origin) {}
void GameOverView::open() {
    m_open = true;
    m_openT = m_t;
    m_focus = 0;
    m_choice = Choice::None;
    m_drawnT = -1;
}
void GameOverView::handle(Command command) {
    if (!m_open || m_t - m_openT < kInputLockFrames) return;
    switch (command) {
        case Command::MoveLeft:
        case Command::Rotate: m_focus = 0; break;
        case Command::MoveRight:
        case Command::SoftDrop: m_focus = 1; break;
        case Command::Confirm:
        case Command::HardDrop:
            m_choice = m_focus == 0 ? Choice::Retry : Choice::Home;
            m_open = false;
            break;
        default: break;
    }
}
GameOverView::Choice GameOverView::takeChoice() {
    Choice c = m_choice;
    m_choice = Choice::None;
    return c;
}
void GameOverView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) {
        m_accMs -= kFrameMs;
        ++m_t;
    }
}
void GameOverView::draw(Console& console) const {
    if (!m_open || m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
void GameOverView::render() const {
    resetPalette();
    Frame& f = m_frame;
    f.gradient(0, 0, Frame::W, Frame::H, { {16, 14, 28}, {40, 32, 58} });
    for (int x = 0; x < Frame::W / 8; ++x) {
        int rows = 2 + static_cast<int>((static_cast<unsigned>(x) * 2654435761u >> 7) % 5);
        for (int r = 0; r < rows; ++r) ToyBlocks::cell8(f, x * 8, Frame::H - (r + 1) * 8, 7);
    }
    int a = m_t - m_openT;
    if (a >= 6 || a % 2 == 0) bigText(f, "GAME OVER", Frame::W / 2, 52, 5, C(230, 70, 70), C(80, 20, 34));
    if (a >= 8) Font::center(f, "THE MEMORY SLIPPED AWAY...", Frame::W / 2, 102, C(204, 198, 226), C(20, 16, 34));
    if (a >= 12) {
        button(f, Frame::W / 2 - 80, 140, 120, "RETRY", m_focus == 0, m_t);
        button(f, Frame::W / 2 + 80, 140, 120, "BACK TO START", m_focus == 1, m_t);
        Font::center(f, "LEFT RIGHT - MOVE   ENTER - SELECT", Frame::W / 2, 178, C(150, 146, 176), C(20, 16, 34));
    }
}
