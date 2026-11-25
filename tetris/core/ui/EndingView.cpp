#include "ui/EndingView.h"
#include "ui/HouseRooms.h"
#include "ui/PixelFont.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace Stage3Art;
static const Look SON{ {40, 34, 40}, {70, 60, 66}, {90, 130, 110}, {66, 100, 84}, {60, 64, 84}, {44, 48, 64}, {250, 250, 246}, false, false, false, false, false, false };
EndingView::EndingView(Point origin) : View(origin) {}
void EndingView::start() { m_started = m_t; }
void EndingView::handle(Command command) {
    if (m_started < 0) return;
    int e = m_t - m_started;
    if ((command == Command::Confirm || command == Command::HardDrop) && e < kFade && e > kEnter) m_started = m_t - std::min(kFade, ((e - kEnter) / 24 + 1) * 24 + kEnter);
}
void EndingView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) { m_accMs -= kFrameMs; ++m_t; }
}
void EndingView::render() const {
    resetPalette();
    std::fill(m_frame.px.begin(), m_frame.px.end(), 0);
    static Frame world;
    int e = m_started < 0 ? 0 : m_t - m_started, t = m_t;
    HouseRooms::draw(world, HouseRooms::LIBRARY, t, false);
    uint8_t ol = C(OLC), red = C(150, 58, 70), redD = C(118, 40, 54), gold = C(222, 184, 92), page = C(250, 246, 232);
    int dadX = 100, dadY = 112;
    chair(world, dadX, dadY, false, false, t);
    int hx = 0, hy = 0;
    Mood m = e >= kAnswer ? SMILE : e >= kAsk ? TALK : CALM;
    person(world, dadX, dadY, HERO_OLD, e >= kEnter + 10 ? false : false, SIT, e < kPut ? HOLD : e >= kHug && e < kFade ? OUT : REST, m, t, 1, &hx, &hy);
    if (e < kClose) { world.rect(hx - 8, hy - 3, 16, 6, ol); world.rect(hx - 7, hy - 2, 7, 4, page); world.rect(hx + 1, hy - 2, 6, 4, page); world.vline(hx, hy - 2, hy + 1, red); if ((t / 3) % 2) star(world, hx + 4, hy - 6); }
    else if (e < kPut) { float u = (e - kClose) / (float)(kPut - kClose); int bx = (int)std::lround(hx + (72 - hx) * u), by = (int)std::lround(hy + (70 - hy) * u - std::sin(u * 3.14f) * 6); world.rect(bx - 5, by - 3, 11, 7, ol); world.rect(bx - 4, by - 2, 9, 5, red); world.rect(bx - 4, by - 2, 2, 5, redD); world.rect(bx, by - 1, 3, 2, gold); }
    else { world.rect(67, 67, 11, 7, ol); world.rect(68, 68, 9, 5, red); world.rect(68, 68, 2, 5, redD); world.rect(72, 69, 3, 2, gold); }
    if (e >= kEnter) {
        float u = std::min(1.0f, (e - kEnter) / (float)kWalk);
        int dx = (int)std::lround(120 + (124 - 120) * u), dy = (int)std::lround(134 + (110 - 134) * u);
        int sx = (int)std::lround(132 + (146 - 132) * u), sy = (int)std::lround(138 + (116 - 138) * u);
        bool walk = u < 1.0f;
        person(world, sx, sy, SON, true, walk ? WALK : STAND, e >= kHug && e < kFade ? OUT : REST, e >= kAnswer ? SMILE : CALM, t + 3, 1);
        person(world, dx, dy, DAUGHTER, true, walk ? WALK : STAND, e >= kHug && e < kFade ? OUT : REST, e >= kAsk && e < kAnswer ? TALK : e >= kAnswer ? SMILE : CALM, t, 1);
    }
    if (e >= kHug && e < kFade) for (int k = 0; k < 3; ++k) { int x = 112 + k * 6, y = 90 - (e - kHug) / 2 - k * 3; uint8_t c = C(240, 90, 120); world.set(x - 1, y, c); world.set(x + 1, y, c); world.hline(x - 1, x + 1, y + 1, c); world.set(x, y + 2, c); }
    HouseRooms::zoomOut(m_frame, world);
    auto box = [&](const std::string& text, int shown) {
        int bx = 30, by = 196, bw = 420, bh = 64;
        m_frame.rect(bx, by, bw, bh, C(250, 250, 250)); m_frame.rect(bx + 3, by + 3, bw - 6, bh - 6, C(10, 8, 12));
        Font::draw(m_frame, text.substr(0, std::min((int)text.size(), shown)), bx + 16, by + 12, C(250, 250, 250));
    };
    if (e >= kAsk && e < kAnswer) box("Dad, how do you feel now?", (e - kAsk) * 2);
    if (e >= kAnswer && e < kHug) box("...Better. Much better.", (e - kAnswer) * 2);
    if (e < 6) { float k = 1 - e / 6.0f; for (auto& c : palette()) c = mix(c, RGB{ 255, 250, 236 }, k); }
    if (e >= kFade) {
        float k = std::min(1.0f, (e - kFade) / 14.0f);
        for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k);
        if (e >= kFade + 14) Font::center(m_frame, "THE END", Frame::W / 2, Frame::H / 2 - 4, C(250, 236, 200));
    }
}
void EndingView::draw(Console& console) const {
    if (m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
