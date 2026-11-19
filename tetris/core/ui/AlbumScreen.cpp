#include "ui/AlbumScreen.h"
#include "ui/PixelFont.h"
#include <algorithm>
#include <cmath>
static const RGB OL{ 42, 26, 28 };
static const char* NAMES[AlbumScreen::STAGES][AlbumScreen::SLOTS] = { { "THE LAKE", "DINNER", "THE BEACH", "BEDTIME" }, { "FIRST DAY", "SOCCER", "???", "???" } };
static const char* MEMORIES[AlbumScreen::STAGES][AlbumScreen::SLOTS] = { { "THE BOAT", "FISH SOUP", "BUCKET", "GOODNIGHT" }, { "MY NOTES", "THE GOAL", "", "" } };
void AlbumScreen::open(int cleared, int t) {
    open_ = true; openT_ = t; detail_ = false;
    newSlot_ = -1;
    if (cleared >= 0 && cleared < SLOTS) { newSlot_ = done_[stage_][cleared] ? -1 : cleared; done_[stage_][cleared] = true; }
    unlocked_ = 0;
    while (unlocked_ < available(stage_) - 1 && done_[stage_][unlocked_]) ++unlocked_;
    sel_ = std::min(cleared + 1, SLOTS - 1);
    sel_ = std::min(sel_, unlocked_);
}
void AlbumScreen::left() { if (sel_ > 0) --sel_; }
void AlbumScreen::right() { if (sel_ < (detail_ ? available(stage_) - 1 : unlocked_)) ++sel_; }
void AlbumScreen::drawIcon(Frame& f, int slot, int cx, int cy) {
    uint8_t ol = C(OL);
    if (slot == 0) {
        f.gradient(cx - 26, cy - 18, 52, 36, { {150, 204, 236}, {96, 160, 210} });
        f.poly({ {cx - 16, cy + 2}, {cx + 16, cy + 2}, {cx + 11, cy + 9}, {cx - 11, cy + 9} }, ol);
        f.poly({ {cx - 15, cy + 3}, {cx + 15, cy + 3}, {cx + 10, cy + 8}, {cx - 10, cy + 8} }, C(156, 96, 56));
        f.hline(cx - 14, cx + 14, cy + 3, C(200, 140, 84));
        f.disc(cx - 2, cy - 4, 4, ol); f.disc(cx - 2, cy - 4, 3, C(246, 204, 164)); f.rect(cx - 5, cy - 8, 7, 3, C(110, 70, 44));
        f.rect(cx - 5, cy - 1, 7, 4, C(240, 200, 70));
        f.line(cx + 4, cy - 2, cx + 14, cy - 10, ol);
        for (int k = 0; k < 4; ++k) f.hline(cx - 22 + k * 12, cx - 17 + k * 12, cy + 13, C(200, 230, 246));
    } else if (slot == 1) {
        f.rect(cx - 26, cy - 18, 52, 36, C(236, 210, 170));
        f.ellipse(cx, cy + 4, 18, 8, ol); f.ellipse(cx, cy + 4, 17, 7, C(250, 250, 244));
        f.ellipse(cx, cy + 1, 14, 4, C(236, 180, 90));
        f.line(cx - 8, cy + 1, cx + 8, cy + 1, C(250, 250, 250)); for (int k = -6; k <= 6; k += 3) f.vline(cx + k, cy - 1, cy + 3, C(250, 250, 250));
        f.poly({ {cx + 8, cy + 1}, {cx + 12, cy - 2}, {cx + 12, cy + 4} }, C(250, 250, 250));
        for (int k = 0; k < 3; ++k) { f.set(cx - 6 + k * 6, cy - 7 - k % 2, C(250, 250, 250)); f.set(cx - 5 + k * 6, cy - 10 + k % 2, C(250, 250, 250)); }
        f.line(cx + 14, cy - 12, cx + 22, cy + 6, C(150, 150, 160));
    } else if (slot == 2) {
        f.gradient(cx - 26, cy - 18, 52, 22, { {250, 196, 140}, {244, 160, 120} });
        f.rect(cx - 26, cy + 4, 52, 14, C(236, 206, 150));
        f.poly({ {cx - 10, cy - 6}, {cx + 4, cy - 6}, {cx + 2, cy + 10}, {cx - 8, cy + 10} }, ol);
        f.poly({ {cx - 9, cy - 5}, {cx + 3, cy - 5}, {cx + 1, cy + 9}, {cx - 7, cy + 9} }, C(220, 70, 70));
        f.hline(cx - 9, cx + 3, cy - 5, C(250, 130, 120));
        f.line(cx - 9, cy - 6, cx - 3, cy - 12, ol); f.line(cx - 3, cy - 12, cx + 3, cy - 6, ol);
        f.line(cx + 8, cy - 10, cx + 12, cy + 6, ol); f.rect(cx + 10, cy + 4, 6, 6, ol); f.rect(cx + 11, cy + 5, 4, 4, C(80, 140, 220));
        f.disc(cx - 18, cy + 10, 2, C(240, 120, 90));
    } else if (slot == 3) {
        f.gradient(cx - 26, cy - 18, 52, 36, { {30, 34, 80}, {60, 60, 120} });
        f.disc(cx + 12, cy - 8, 7, C(250, 236, 170)); f.disc(cx + 15, cy - 10, 6, C(40, 44, 92));
        for (int k = 0; k < 6; ++k) f.set(cx - 22 + k * 8, cy - 14 + (k % 3) * 5, C(250, 250, 220));
        f.rect(cx - 20, cy + 6, 40, 10, ol); f.rect(cx - 19, cy + 7, 38, 8, C(120, 160, 220)); f.rect(cx - 19, cy + 7, 10, 4, C(250, 250, 244));
        f.disc(cx - 12, cy + 4, 4, ol); f.disc(cx - 12, cy + 4, 3, C(246, 204, 164));
    } else if (slot == 4) {
        f.rect(cx - 26, cy - 18, 52, 36, C(214, 172, 114));
        f.rect(cx - 16, cy - 12, 32, 24, ol); f.rect(cx - 15, cy - 11, 14, 22, C(252, 250, 240)); f.rect(cx + 1, cy - 11, 14, 22, C(252, 250, 240)); f.vline(cx, cy - 11, cy + 10, C(150, 58, 70));
        for (int k = 0; k < 5; ++k) { f.hline(cx - 13, cx - 3, cy - 8 + k * 4, C(60, 70, 120)); f.hline(cx + 3, cx + 13, cy - 8 + k * 4, C(170, 196, 230)); f.set(cx - 14, cy - 8 + k * 4, C(210, 70, 70)); }
        f.line(cx + 10, cy + 8, cx + 20, cy - 6, C(40, 60, 140));
    } else if (slot == 5) {
        f.gradient(cx - 26, cy - 18, 52, 36, { {130, 196, 240}, {118, 186, 84} });
        f.rect(cx + 6, cy - 14, 2, 22, C(250, 250, 250)); f.rect(cx + 6, cy - 14, 18, 2, C(250, 250, 250));
        for (int k = 0; k < 5; ++k) f.line(cx + 8 + k * 4, cy - 12, cx + 8 + k * 4, cy + 6, C(214, 226, 232));
        f.disc(cx - 8, cy + 2, 8, ol); f.disc(cx - 8, cy + 2, 7, C(250, 250, 250)); f.disc(cx - 8, cy + 2, 2, ol); f.set(cx - 12, cy - 2, ol); f.set(cx - 4, cy + 6, ol); f.set(cx - 13, cy + 5, ol);
    }
}
static void rotBlit(Frame& f, const Frame& src, uint8_t key, int w, int h, int pinX, int pinY, float ang) {
    float c = std::cos(ang), sn = std::sin(ang);
    int r = (int)std::ceil(std::sqrt((float)(w * w + h * h))) + 2;
    for (int y = pinY - 2; y <= pinY + r; ++y) for (int x = pinX - r; x <= pinX + r; ++x) {
        float dx = x - pinX, dy = y - pinY, u = dx * c + dy * sn + w / 2.0f, v = -dx * sn + dy * c;
        if (u < 0 || v < 0 || u >= w || v >= h) continue;
        uint8_t col = src.get((int)u, (int)v);
        if (col != key) f.set(x, y, col);
    }
}
static void corner(int pinX, int pinY, float ang, float u, float v, int& x, int& y) {
    float c = std::cos(ang), sn = std::sin(ang);
    x = (int)std::lround(pinX + u * c - v * sn); y = (int)std::lround(pinY + u * sn + v * c);
}
void AlbumScreen::draw(Frame& f, int t, const Frame* preview) const {
    if (detail_) { drawDetail(f, t, preview); return; }
    int a = t - openT_;
    uint8_t ol = C(OL), gold = C(222, 184, 92);
    f.gradient(0, 0, Frame::W, Frame::H, { {30, 22, 26}, {18, 12, 16} });
    int lift = (int)std::lround(std::max(0.0f, 1 - a / 8.0f) * 50);
    auto Y = [&](float y) { return (int)std::lround(y) + lift; };
    const int L = 58, M = 240, R = 422;
    auto topY = [&](int x) { float u = std::abs(x - M) / (float)(M - L); return 34.0f + 30.0f * std::pow(1 - u, 2.2f); };
    auto botY = [&](int x) { float u = std::abs(x - M) / (float)(M - L); return 212.0f + 14.0f * (1 - u) * (1 - u) - 2.0f * u; };
    uint8_t cover = C(120, 76, 50), coverD = C(84, 50, 36), edge1 = C(236, 222, 190), edge2 = C(200, 180, 144);
    for (int x = L - 8; x <= R + 8; ++x) {
        int xx = std::max(L, std::min(R, x));
        int yb = Y(botY(xx)) + 16;
        f.vline(x, Y(topY(xx)) + 6, yb + 4, ol);
        f.vline(x, yb - 2, yb + 2, cover); f.set(x, yb + 3, coverD);
    }
    for (int k = 6; k >= 1; --k) for (int x = L - k; x <= R + k; ++x) {
        int xx = std::max(L, std::min(R, x));
        int y = Y(botY(xx)) + k * 2;
        f.set(x, y, k % 2 ? edge1 : edge2); f.set(x, y + 1, k % 2 ? edge2 : edge1);
    }
    for (int x = L; x <= R; ++x) {
        float u = std::abs(x - M) / (float)(M - L);
        int y0 = Y(topY(x)), y1 = Y(botY(x));
        RGB pc = mix(RGB{ 248, 240, 216 }, RGB{ 214, 196, 160 }, std::pow(1 - u, 6.0f) * 0.9f);
        if (x < M) pc = mix(pc, RGB{ 236, 224, 196 }, 0.25f);
        f.vline(x, y0, y1, C(pc));
        f.set(x, y0, ol); f.set(x, y0 + 1, C(252, 248, 232));
    }
    for (int k = 0; k < 4; ++k) { f.vline(L - 1 - k, Y(topY(L)) + 2 + k, Y(botY(L)) + k, k % 2 ? edge2 : edge1); f.vline(R + 1 + k, Y(topY(R)) + 2 + k, Y(botY(R)) + k, k % 2 ? edge2 : edge1); }
    f.vline(L - 5, Y(topY(L)) + 5, Y(botY(L)) + 6, ol); f.vline(R + 5, Y(topY(R)) + 5, Y(botY(R)) + 6, ol);
    f.vline(M, Y(topY(M)) + 1, Y(botY(M)) + 12, C(150, 120, 90)); f.vline(M - 1, Y(topY(M)) + 2, Y(botY(M)), C(190, 166, 130));
    uint8_t rib = C(70, 140, 160), ribD = C(44, 96, 116);
    f.rect(M - 3, Y(botY(M)) + 12, 7, 14, ol); f.rect(M - 2, Y(botY(M)) + 12, 5, 13, rib); f.vline(M + 2, Y(botY(M)) + 12, Y(botY(M)) + 24, ribD);
    f.set(M, Y(botY(M)) + 24, ol); f.set(M, Y(botY(M)) + 23, ol);
    uint8_t twine = C(150, 110, 70), twineD = C(110, 78, 50);
    float pinU[SLOTS];
    int pinX[SLOTS], pinY[SLOTS];
    for (int page = 0; page < 2; ++page) {
        int x0 = page == 0 ? 82 : 258, x1 = page == 0 ? 222 : 398;
        for (int x = x0; x <= x1; ++x) {
            float u = (x - x0) / (float)(x1 - x0);
            float y = 86 + 12 * (1 - (2 * u - 1) * (2 * u - 1));
            f.set(x, Y(y), twine); f.set(x, Y(y) + 1, twineD);
        }
        f.disc(x0, Y(86), 2, ol); f.disc(x0, Y(86), 1, C(200, 60, 60)); f.disc(x1, Y(86), 2, ol); f.disc(x1, Y(86), 1, C(200, 60, 60));
        for (int j = 0; j < 2; ++j) {
            int s = page * 2 + j;
            pinU[s] = j == 0 ? 0.28f : 0.74f;
            pinX[s] = (int)std::lround(x0 + (x1 - x0) * pinU[s]);
            float u = pinU[s];
            pinY[s] = Y(86 + 12 * (1 - (2 * u - 1) * (2 * u - 1)));
        }
    }
    static Frame card;
    uint8_t key = C(255, 0, 255);
    const int CW = 58, CH = 66;
    for (int s = 0; s < SLOTS; ++s) {
        int fl = flipT_ >= 0 ? t - flipT_ : -1;
        int ps = flipT_ < 0 ? stage_ : (s >= 2 ? 1 : (fl >= 28 ? 1 : 0));
        bool done = done_[ps][s], open = ps == stage_ ? s <= unlocked_ : (ps == 0 || s == 0), sel = s == sel_ && flipT_ < 0, pop = s == newSlot_ && a < 16 && flipT_ < 0;
        if (ps == 1 && s >= available(1)) open = false;
        float ang = std::sin(t * 0.18f + s * 1.7f) * (sel ? 0.08f : 0.035f) + (s % 2 ? 0.05f : -0.05f);
        card.rect(0, 0, CW, CH, key);
        card.rect(0, 0, CW, CH, ol); card.rect(1, 1, CW - 2, CH - 2, C(252, 250, 244)); card.hline(1, CW - 2, CH - 2, C(220, 214, 200));
        card.rect(4, 4, CW - 8, 44, ol);
        if (done && !(pop && a < 6)) { Frame tmp; tmp.rect(0, 0, 60, 46, key); drawIcon(tmp, ps * 4 + s, 30, 22); for (int y = 0; y < 42; ++y) for (int x = 0; x < CW - 10; ++x) { uint8_t c = tmp.get(x + 6, y + 2); if (c != key) card.set(5 + x, 5 + y, c); } }
        else card.rect(5, 5, CW - 10, 42, open ? C(206, 196, 176) : C(140, 132, 122));
        if (!done && open) Font::center(card, "?", CW / 2, 22, C(120, 100, 90));
        if (!open) { int cx = CW / 2, cy = 24; card.rect(cx - 5, cy - 2, 10, 8, ol); card.rect(cx - 4, cy - 1, 8, 6, gold); card.hline(cx - 3, cx + 3, cy - 6, ol); card.vline(cx - 3, cy - 6, cy - 2, ol); card.vline(cx + 3, cy - 6, cy - 2, ol); }
        Font::center(card, std::string("ROUND ") + char('1' + s), CW / 2, 53, done ? C(150, 58, 70) : C(140, 124, 110));
        int px = pinX[s], py = pinY[s] + 3;
        rotBlit(f, card, key, CW, CH, px, py, ang);
        if (sel && a >= 8) {
            uint8_t hl = (t / 3) % 2 ? gold : C(250, 226, 130);
            int cx[4], cy[4];
            corner(px, py, ang, -CW / 2.0f - 3, -3, cx[0], cy[0]); corner(px, py, ang, CW / 2.0f + 3, -3, cx[1], cy[1]);
            corner(px, py, ang, CW / 2.0f + 3, CH + 3, cx[2], cy[2]); corner(px, py, ang, -CW / 2.0f - 3, CH + 3, cx[3], cy[3]);
            for (int k = 0; k < 4; ++k) { int n = (k + 1) % 4; f.line(cx[k], cy[k], cx[n], cy[n], hl); f.line(cx[k], cy[k] + 1, cx[n], cy[n] + 1, hl); }
        }
        f.rect(px - 2, py - 6, 5, 11, ol); f.rect(px - 1, py - 5, 3, 9, C(214, 170, 110)); f.vline(px, py - 5, py + 3, C(170, 126, 80)); f.set(px - 1, py - 1, C(200, 200, 206));
        int capY = Y(186) + (s % 2) * 2;
        Font::center(f, done ? MEMORIES[ps][s] : (open ? "..." : ""), px, capY, C(130, 90, 70));
        if (done) f.hline(px - 18, px + 18, capY + 9, C(200, 170, 140));
        if (pop) for (int k = 0; k < 8; ++k) { float an = k * 0.785f + a * 0.3f; int r = 24 + a; f.set(px + (int)(std::cos(an) * r), py + 34 + (int)(std::sin(an) * r * 0.8f), gold); }
    }
    if (flipT_ >= 0) {
        int fl = t - flipT_;
        auto pageCol = [&](int x, float shade) {
            int y0 = Y(topY(x)) - 3, y1 = Y(botY(x));
            f.vline(x, y0, y1, C(mix(RGB{ 244, 234, 210 }, RGB{ 190, 170, 136 }, shade)));
            f.set(x, y0, ol); f.set(x, y1, ol);
        };
        if (fl < 14) { float u = fl / 14.0f; int xe = (int)std::lround(R - (R - M) * u); for (int x = M + 1; x <= xe; ++x) pageCol(x, 0.15f + 0.5f * u * (x - M) / (float)std::max(1, xe - M)); f.vline(xe, Y(topY(xe)) - 3, Y(botY(xe)), ol); }
        else if (fl < 28) { float u = (fl - 14) / 14.0f; int xe = (int)std::lround(M - (M - L) * u); for (int x = xe; x < M; ++x) pageCol(x, 0.5f * (1 - u) * (M - x) / (float)std::max(1, M - xe)); f.vline(xe, Y(topY(xe)) - 3, Y(botY(xe)), ol); }
        if (fl >= 32) {
            std::string msg = "Eat something first...";
            int n = std::min((int)msg.size(), (fl - 32) / 1 + 1);
            Font::center(f, msg.substr(0, n), 150, Y(202), C(150, 58, 70));
            if (n == (int)msg.size()) f.hline(150 - Font::width(msg) / 2, 150 + Font::width(msg) / 2, Y(211), C(200, 170, 140));
        }
        Font::center(f, "MEMORY BOOK", Frame::W / 2, 8, gold);
    } else if (a >= 8) {
        bool done = done_[stage_][sel_];
        std::string line = std::string("ROUND ") + char('1' + sel_) + "  " + NAMES[stage_][sel_] + "  -  " + (done ? MEMORIES[stage_][sel_] : "NEXT");
        Font::center(f, line, Frame::W / 2, 254, C(250, 236, 200));
        Font::center(f, "MEMORY BOOK", Frame::W / 2, 8, gold);
    }
    if (a < 10 && flipT_ < 0) {
        static Frame copy; copy.px = f.px;
        float z = 1 + 0.7f * (1 - a / 10.0f);
        int cx = newSlot_ >= 0 ? (newSlot_ < 2 ? 152 : 328) : 240, cy = 135;
        for (int y = 0; y < Frame::H; ++y) for (int x = 0; x < Frame::W; ++x) f.set(x, y, copy.get(cx + (int)((x - cx) / z), cy + (int)((y - cy) / z)));
    }
    if (flipT_ >= 0 && t - flipT_ > FLIP_END - 10) { float k = std::min(1.0f, (t - flipT_ - FLIP_END + 10) / 10.0f); for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k); }
    if (a < 8) { float k = 1 - a / 8.0f; for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k); }
}
void AlbumScreen::drawDetail(Frame& f, int t, const Frame* preview) const {
    uint8_t ol = C(OL), gold = C(222, 184, 92), paper = C(252, 250, 244), ink = C(90, 76, 70);
    f.rect(0, 0, Frame::W, Frame::H, C(20, 14, 18));
    bool open = sel_ <= unlocked_, done = done_[stage_][sel_];
    const int PW = 288, PH = 162, px = (Frame::W - PW) / 2, py = 30;
    Font::center(f, std::string("ROUND ") + char('1' + sel_) + "  -  " + NAMES[stage_][sel_], Frame::W / 2, 10, C(250, 236, 200));
    f.rect(px - 9, py - 9, PW + 18, PH + 30, ol); f.rect(px - 8, py - 8, PW + 16, PH + 28, paper); f.hline(px - 8, px + PW + 7, py + PH + 19, C(220, 214, 200));
    f.rect(px - 1, py - 1, PW + 2, PH + 2, ol);
    if (preview) {
        for (int y = 0; y < PH; ++y) for (int x = 0; x < PW; ++x) f.set(px + x, py + y, preview->get(x * Frame::W / PW, y * Frame::H / PH));
    }
    if (!open) {
        for (int y = 0; y < PH; ++y) for (int x = 0; x < PW; ++x) if ((x + y) % 2 == 0) f.set(px + x, py + y, C(30, 24, 28));
        int cx = Frame::W / 2, cy = py + PH / 2;
        f.rect(cx - 10, cy - 4, 20, 16, ol); f.rect(cx - 9, cy - 3, 18, 14, gold); f.rect(cx - 7, cy - 14, 14, 3, ol); f.rect(cx - 7, cy - 14, 3, 11, ol); f.rect(cx + 4, cy - 14, 3, 11, ol); f.rect(cx - 1, cy + 2, 2, 5, ol);
    }
    Font::center(f, done ? MEMORIES[stage_][sel_] : (open ? "NEXT" : "LOCKED"), Frame::W / 2, py + PH + 7, done ? C(90, 120, 70) : ink);
    int bob = (t / 4) % 2;
    if (sel_ > 0) { int ax = px - 34 - bob, ay = py + PH / 2; f.poly({ {ax, ay}, {ax + 14, ay - 14}, {ax + 14, ay + 14} }, ol); f.poly({ {ax + 3, ay}, {ax + 12, ay - 9}, {ax + 12, ay + 9} }, gold); }
    if (sel_ < available(stage_) - 1) { int ax = px + PW + 34 + bob, ay = py + PH / 2; f.poly({ {ax, ay}, {ax - 14, ay - 14}, {ax - 14, ay + 14} }, ol); f.poly({ {ax - 3, ay}, {ax - 12, ay - 9}, {ax - 12, ay + 9} }, gold); }
    const char* label[2] = { "CANCEL", "PLAY" };
    for (int b = 0; b < 2; ++b) {
        int bx = b == 0 ? px : px + PW - 84, by = 236;
        bool focus = focus_ == b, dim = b == 1 && !open;
        uint8_t face = dim ? C(150, 140, 130) : (focus ? C(250, 220, 120) : C(214, 160, 96)), edge = focus ? C(255, 244, 200) : C(240, 196, 130);
        f.rect(bx + 2, by + 2, 84, 22, C(10, 6, 8));
        f.rect(bx, by, 84, 22, ol); f.rect(bx + 1, by + 1, 82, 20, face); f.hline(bx + 1, bx + 82, by + 1, edge); f.hline(bx + 1, bx + 82, by + 20, C(160, 108, 62));
        Font::center(f, label[b], bx + 42, by + 8, dim ? C(110, 100, 94) : ol);
        if (focus && (t / 3) % 2) { f.rect(bx - 3, by - 3, 90, 2, gold); f.rect(bx - 3, by + 23, 90, 2, gold); f.rect(bx - 3, by - 3, 2, 28, gold); f.rect(bx + 87, by - 3, 2, 28, gold); }
    }
}
