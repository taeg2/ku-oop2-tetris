// BedroomRound.cpp - Round 4, the boss. Everything is drawn here: the room, the window, the goo, the tentacles
// and the mist with code (shapes, lines, fills), the kid, mom, dad and the toy soldiers from
// hand-typed sprites (one character = one color). No image files, no converted data.
#include "BedroomRound.h"
#include "People.h"
#include "Font.h"
#include "BedroomArt.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
const RGB OUT{ 42, 26, 28 };
const RGB GOO0{ 10, 6, 16 }, GOO2{ 46, 30, 66 }, GOO3{ 90, 66, 120 };
const RGB WALL{ 70, 80, 136 }, WALL_D{ 58, 66, 118 }, WALL_L{ 84, 96, 152 }, CEIL{ 40, 42, 74 };
const RGB WOOD{ 128, 84, 58 }, WOOD_D{ 96, 60, 42 }, WOOD_L{ 160, 110, 76 };
const int BX = 96, BY = 80;
const int FLOOR_Y = 124;                                          // where the back wall meets the floor
const float PI = 3.14159265f;
const int BAYER[4][4] = { {0,8,2,10}, {12,4,14,6}, {3,11,1,9}, {15,7,13,5} };
const int SOLDIER_SPOT[20][2] = { {342,226}, {353,226}, {364,226}, {375,226}, {386,226}, {333,214}, {343,214}, {352,214}, {361,214}, {371,214},
                                  {325,202}, {333,202}, {340,202}, {348,202}, {356,202}, {317,190}, {323,190}, {329,190}, {335,190}, {341,190} };
const float CHEST[8][2] = { {6,193}, {62,166}, {38,193}, {86,166}, {6,153}, {62,136}, {38,153}, {86,136} };   // bottom: bn bf fn ff, top: tbn tbf tfn tff
const int BED_FL[2] = { 205,190 }, BED_FR[2] = { 275,190 }, BED_HL[2] = { 182,264 }, BED_HR[2] = { 298,264 };   // foot far away, head near us
const int HOOD_X = 251, HOOD_Y = 72;                              // the mouth of the hood on its hook
const int DOOR_X0 = 276, DOOR_X1 = 318, DOOR_Y0 = 46;
const int COCOON_X = 215, COCOON_Y = 150;

float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
float ease(float t) { t = clamp01(t); return 0.5f - 0.5f * std::cos(PI * t); }
float lerp(float a, float b, float u) { return a + (b - a) * u; }
int rr(float v) { return (int)std::lround(v); }
float hash01(unsigned a) { a = a * 2654435761u ^ (a >> 13); a *= 0x5bd1e995u; a ^= a >> 15; return (a & 0xFFFF) / 65535.0f; }
bool inPoly(const float (*p)[2], int n, float x, float y) {
    bool in = false;
    for (int i = 0, j = n - 1; i < n; j = i++)
        if (((p[i][1] > y) != (p[j][1] > y)) && (x < (p[j][0] - p[i][0]) * (y - p[i][1]) / (p[j][1] - p[i][1]) + p[i][0])) in = !in;
    return in;
}
void disc(Frame& f, float cx, float cy, float r, uint8_t c) {
    for (int y = (int)std::floor(cy - r); y <= (int)std::ceil(cy + r); ++y)
        for (int x = (int)std::floor(cx - r); x <= (int)std::ceil(cx + r); ++x) {
            float dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= r * r + 0.25f) f.set(x, y, c);
        }
}
void quad(Frame& f, float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy, uint8_t c) {
    f.poly({ {rr(ax), rr(ay)}, {rr(bx), rr(by)}, {rr(cx), rr(cy)}, {rr(dx), rr(dy)} }, c);
}
// a speech bubble with a tail pointing at (tx, ty)
void bubble(Frame& f, int cx, int y, const char* l1, const char* l2, int tx, int ty) {
    int w = std::max(Font::width(l1), Font::width(l2)) + 8, h = l2[0] ? 20 : 12, x = std::max(2, std::min(477 - w, cx - w / 2));
    uint8_t ol = C(OUT), paper = C(252, 250, 244);
    int bx = std::max(x + 6, std::min(x + w - 10, tx - 3));
    f.poly({ {bx, y + h - 1}, {bx + 7, y + h - 1}, {tx, ty} }, paper);
    f.line(bx, y + h, tx, ty, ol); f.line(bx + 7, y + h, tx, ty, ol);
    f.rect(x - 1, y, w + 2, h, ol); f.rect(x, y - 1, w, h + 2, ol);
    f.rect(x, y, w, h, paper);
    f.hline(bx + 1, bx + 6, y + h, paper);
    Font::center(f, l1, x + w / 2, y + 3, C(60, 40, 60));
    if (l2[0]) Font::center(f, l2, x + w / 2, y + 11, C(60, 40, 60));
}

// ---------------------------------------------------------------- hand-typed sprites (one character = one color)
// a toy soldier (7 x 14)
const Sprite SOLDIER("toy soldier", {
    "..OOO..",
    ".OKKKO.",
    ".OKKKO.",
    ".OKKKO.",
    "OOGGGOO",
    ".OSSSO.",
    ".OESEO.",
    "..OSO..",
    ".ORRRO.",
    "ORRWRRO",
    ".ORRRO.",
    ".OBOBO.",
    ".OBOBO.",
    ".OKOKO.",
}, { {'O', {30, 20, 26}}, {'K', {40, 40, 56}}, {'G', {236, 196, 80}}, {'S', {246, 204, 164}}, {'E', {34, 26, 34}},
     {'R', {200, 50, 56}}, {'W', {246, 240, 228}}, {'B', {60, 80, 150}} });
// dad: short hair, glasses, blue shirt (16 wide; head 15 rows, body 27 rows)
const Sprite DAD_HEAD("dad head", {
    "....OOOOOOOO....",
    "...OhHHHHHHHO...",
    "..OHHHHHHHHHhO..",
    "..OHHHHHHHHHhO..",
    "..OHSSSSSSSShO..",
    "..OSSSSSSSSSSO..",
    "..OSGGGSSGGGSO..",
    "..OSGEGGGGEGSO..",
    "..OSGGGSSGGGSO..",
    "..OSSSSSSSSSSO..",
    "..OSSSSMMSSSSO..",
    "...OSSSSSSSSO...",
    "....OOSSSSOO....",
    "......OSSO......",
    "......OSSO......",
}, { {'O', {42, 26, 28}}, {'H', {70, 50, 40}}, {'h', {50, 36, 30}}, {'S', {240, 196, 156}}, {'G', {60, 60, 70}}, {'E', {34, 26, 34}}, {'M', {176, 80, 84}} });
const Sprite DAD_BODY("dad body", {
    "...OOOCCCCOOO...",
    "..OCCCCCCCCCCO..",
    ".OCCOCCCCCCOCcO.",
    ".OCCOCCCCCCOCcO.",
    ".OCCOCCCCCCOCcO.",
    ".OCCOCCCCCCOCcO.",
    ".OCCOCCCCCCOCcO.",
    ".OSSOCCCCCCOSsO.",
    ".OSSOCCCCCCOSsO.",
    "..OOONNNNNNOOO..",
    "....ONNNNNNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONNOONNO....",
    "....ONnOONnO....",
    "....ONnOONnO....",
    "...OKKKOOKKKO...",
    "...OOOOOOOOOOO..",
}, { {'O', {42, 26, 28}}, {'C', {150, 190, 230}}, {'c', {110, 150, 200}}, {'S', {240, 196, 156}}, {'s', {214, 166, 128}},
     {'N', {60, 62, 92}}, {'n', {44, 46, 70}}, {'K', {50, 36, 34}} });
void blitFrame(Frame& f, const Sprite& sp, int fw, int k, int x, int y, int clipY = 100000, const Sprite* mask = nullptr, int mfw = 0, int mk = 0, int mx = 0, int my = 0, char mch = 0) {
    int lut[128];
    for (int i = 0; i < 128; ++i) lut[i] = -2;
    for (int r = 0; r < sp.h; ++r) {
        int py = y + r;
        if (py > clipY) break;
        const std::string& row = sp.rows[r];
        for (int c = 0; c < fw; ++c) {
            char ch = row[k * fw + c];
            if (ch == '.') continue;
            int px = x + c;
            if (mask) {
                int lx = px - mx, ly = py - my;
                if (lx < 0 || ly < 0 || lx >= mfw || ly >= mask->h || mask->rows[ly][mk * mfw + lx] != mch) continue;
            }
            int& l = lut[(int)ch & 127];
            if (l == -2) l = sp.has[(int)ch & 127] ? C(sp.key[(int)ch & 127]) : -1;
            if (l >= 0) f.set(px, py, (uint8_t)l);
        }
    }
}
}

// ---------------------------------------------------------------- the room (walls, floor, window, furniture)
void BedroomRound::drawRoom(Frame& fr) {
    uint8_t wall = C(WALL), wallD = C(WALL_D), wallL = C(WALL_L), ceil = C(CEIL);
    const int RC = 345;
    auto wallEnd = [&](int x) { return x < 104 ? FLOOR_Y + (104 - x) * 66 / 104 : x > RC ? FLOOR_Y + rr((x - RC) * 0.433f) : FLOOR_Y; };
    auto floorTop = [&](int x) { return x > RC ? wallEnd(x) + rr(7 + (x - 390) * 0.057f) : wallEnd(x); };
    for (int x = 0; x < 480; ++x) {
        int yb = wallEnd(x);
        for (int y = 0; y < yb; ++y) {
            uint8_t c = (x < 104 || x > RC) ? wallD : wall;
            if (x >= 104 && x <= RC && (x - 104) % 14 == 0) c = wallD;
            if (x >= 104 && x <= RC && (x - 104) % 14 == 1) c = wallL;
            fr.set(x, y, c);
        }
        if (x > RC) { fr.set(x, yb, C(98, 74, 74)); for (int y = yb + 1; y < floorTop(x); ++y) fr.set(x, y, C(54, 40, 50)); }
    }
    fr.rect(104, 0, RC - 103, 8, ceil);
    fr.poly({ {0, 0}, {104, 0}, {104, 8}, {0, 0} }, ceil);
    fr.vline(104, 8, FLOOR_Y, C(46, 52, 92)); fr.vline(RC, 8, FLOOR_Y, C(46, 52, 92));
    uint8_t fl = C(104, 68, 52), fl2 = C(96, 62, 48), seam = C(70, 44, 36);
    for (int x = 0; x < 480; ++x) {
        int y0 = floorTop(x);
        for (int y = y0; y < 270; ++y) {
            float u = (x - 238) * 146.0f / (y - 20) / 22.0f;
            int b = (int)std::floor(u);
            uint8_t c = (b & 1) ? fl : fl2;
            if (u - b < 0.07f) c = seam;
            fr.set(x, y, c);
        }
        fr.set(x, y0, C(54, 40, 40));
    }
    // round rug in the middle of the room
    fr.ellipse(COCOON_X, COCOON_Y + 2, 30, 10, C(96, 60, 96)); fr.ellipse(COCOON_X, COCOON_Y + 1, 28, 9, C(150, 104, 150));
    fr.ellipse(COCOON_X, COCOON_Y + 1, 20, 6, C(176, 132, 172)); fr.ellipse(COCOON_X, COCOON_Y + 1, 12, 3, C(150, 104, 150));
    // the coat stand with his hat and green raincoat
    uint8_t pole = C(70, 50, 40);
    fr.vline(338, 36, 130, pole); fr.vline(339, 36, 130, C(96, 70, 52));
    fr.line(338, 130, 330, 136, pole); fr.line(339, 130, 347, 136, pole); fr.line(338, 130, 338, 137, pole);
    fr.poly({ {330, 44}, {346, 44}, {350, 96}, {326, 96} }, C(OUT)); fr.poly({ {331, 45}, {345, 45}, {349, 95}, {327, 95} }, C(70, 130, 90));
    fr.line(338, 45, 338, 95, C(50, 100, 70));
    fr.ellipse(338, 40, 10, 2, C(OUT)); fr.ellipse(338, 38, 6, 4, C(OUT)); fr.ellipse(338, 38, 5, 3, C(196, 160, 80)); fr.hline(329, 347, 40, C(170, 136, 64));
}

// the window on the left wall: night sky, the moon (a crescent that swells to full), trees (that turn horrible)
void BedroomRound::drawWindow(Frame& fr, int pf, int ee) {
    static const float GL[4][2] = { {14, 34}, {90, 22}, {90, 104}, {14, 118} };   // the glass, seen at an angle
    float moon = pf >= G0 ? ease((pf - G0) / 12.0f) : 0.0f;
    bool horror = pf >= G0 + 6;
    for (int y = 20; y < 120; ++y)
        for (int x = 14; x <= 90; ++x) {
            if (!inPoly(GL, 4, x + 0.5f, y + 0.5f)) continue;
            int k = (y - 20) / 20;
            RGB sky[5] = { {20, 22, 52}, {26, 30, 66}, {34, 40, 80}, {44, 50, 94}, {52, 58, 104} };
            RGB c = sky[std::min(4, k)];
            if (((y - 20) % 20) > 16 && k < 4 && ((x + y) & 1)) c = sky[k + 1];
            fr.set(x, y, C(c));
        }
    // stars, then the moon
    for (int k = 0; k < 12; ++k) { int x = 18 + (k * 37) % 68, y = 30 + (k * 23) % 50; if (inPoly(GL, 4, x, y)) fr.set(x, y, C(200, 210, 240)); }
    float mr = 6 + moon * 3, mx = 64, my = 44;
    disc(fr, mx, my, mr, C(250, 246, 220));
    if (moon < 1) disc(fr, mx + 4 - moon * 14, my - 2, mr * (1 - moon * 0.5f), C(26, 30, 66));   // the shadow slides off: full moon
    else { fr.set(rr(mx) - 2, rr(my) - 1, C(220, 214, 190)); fr.set(rr(mx) + 2, rr(my) + 2, C(220, 214, 190)); }
    // trees: pines, which become bare, clawing horror trees when the moon is full
    uint8_t tree = C(16, 18, 34);
    for (int k = 0; k < 6; ++k) {
        int tx = 18 + k * 13, base = 112 - (tx - 14) * 14 / 76, h = 26 + (k * 7) % 12;
        if (!horror) {
            for (int d = 0; d < h; ++d) { int hw = d * 3 / 10 + ((d % 6) < 3 ? 0 : -1) + 1; for (int x = tx - hw; x <= tx + hw; ++x) if (inPoly(GL, 4, x, base - h + d)) fr.set(x, base - h + d, tree); }
        } else {
            for (int y = base - h; y <= base; ++y) if (inPoly(GL, 4, tx, y)) fr.set(tx, y, tree);
            for (int b = 0; b < 4; ++b) {                                 // crooked branches like fingers
                int by = base - h + 4 + b * 6, s = (b % 2) ? 1 : -1, L = 6 + (k + b) % 4;
                for (int i = 0; i < L; ++i) { int x = tx + s * i, y = by - i / 2 - (i > L - 3 ? 1 : 0); if (inPoly(GL, 4, x, y)) fr.set(x, y, tree); }
            }
        }
    }
    // rain outside, from the first lightning until the goo dries up
    if (pf >= L0 && !(ee >= GOO_DRY + 12)) {
        unsigned h = (unsigned)pf * 2654435761u;
        for (int k = 0; k < 22; ++k) {
            h = h * 1103515245u + 12345u; int x = 14 + (int)((h >> 8) % 77);
            h = h * 1103515245u + 12345u; int y = 26 + (int)((h >> 8) % 86);
            for (int i = 0; i < 5; ++i) if (inPoly(GL, 4, x - i / 2, y + i)) fr.set(x - i / 2, y + i, C(90, 100, 150));
        }
    }
    // frame, cross bars, sill
    uint8_t frame = C(WOOD_L), frameD = C(WOOD_D), ol = C(OUT);
    fr.line(12, 32, 92, 19, ol); fr.line(12, 120, 92, 106, ol); fr.line(12, 32, 12, 120, ol); fr.line(92, 19, 92, 106, ol);
    fr.line(13, 33, 91, 20, frame); fr.line(13, 119, 91, 105, frame); fr.vline(13, 33, 119, frame); fr.vline(91, 20, 105, frame);
    fr.vline(52, 27, 112, frame); fr.vline(53, 27, 112, frameD);
    fr.line(14, 77, 90, 64, frame); fr.line(14, 78, 90, 65, frameD);
    fr.poly({ {6, 120}, {96, 105}, {98, 110}, {6, 126} }, frame); fr.line(6, 126, 98, 110, ol);
    // curtains: hang still, then blow in once the window is open
    float blow = pf >= WG0 && !(ee >= GOO_DRY) ? 0.5f + 0.5f * std::sin(pf * 0.35f) : 0.0f;
    uint8_t cur = C(96, 50, 96), curD = C(66, 34, 70);
    for (int s = 0; s < 2; ++s) {
        int x0 = s == 0 ? 4 : 82, w = 12;
        for (int y = 16; y < 124; ++y) {
            float d = (y - 16) / 108.0f, off = blow * 18 * d * d * (s == 0 ? 1 : 1) + std::sin(y * 0.3f + pf * 0.5f) * blow * 2;
            int xa = x0 + rr(off), wa = w - rr(d * 3);
            fr.hline(xa, xa + wa, y - (s == 1 ? (x0 + 6) * 0 : 0), cur);
            for (int x = xa + 2; x < xa + wa; x += 4) fr.set(x, y, curD);
            fr.set(xa - 1, y, ol); fr.set(xa + wa + 1, y, ol);
        }
    }
    fr.line(0, 14, 100, 0, C(WOOD_D)); fr.line(0, 15, 100, 1, C(WOOD_D));    // curtain rod
}

// the bookshelf above the hooks; one book is the NEXT book (it flies out in the intro)
void BedroomRound::drawShelf(Frame& fr, int pf) {
    fr.rect(158, 42, 82, 4, C(OUT)); fr.rect(159, 42, 80, 3, C(WOOD)); fr.hline(159, 238, 42, C(WOOD_L));
    static const RGB BOOK[9] = { {200,60,60}, {70,110,190}, {240,200,70}, {90,160,90}, {170,90,170}, {230,130,60}, {70,170,180}, {210,90,120}, {120,120,200} };
    int x = 162;
    for (int k = 0; k < 9; ++k) {
        int w = 5 + (k * 3) % 4, h = 12 + (k * 5) % 6;
        if (k == 4) {                                                     // the NEXT book: gone once it flies out
            if (pf < BOOK_FLY0) {
                int wig = (pf >= B0 && pf < BOOK_FLY0) ? ((pf % 2) ? 1 : -1) : 0, slide = pf >= B0 + 4 ? (pf - B0 - 4) * 2 : 0;
                fr.rect(x + wig, 42 - h - slide / 3, w + 2, h, C(OUT)); fr.rect(x + 1 + wig, 43 - h - slide / 3, w, h - 2, C(250, 230, 120));
            }
        } else { fr.rect(x, 42 - h, w + 1, h, C(OUT)); fr.rect(x + 1, 43 - h, w - 1, h - 2, C(BOOK[k])); fr.hline(x + 1, x + w - 1, 46 - h, C(255, 255, 255)); }
        x += w + 2;
    }
}

void BedroomRound::drawDoor(Frame& fr, int t) {
    int pf = f(t), ee = e(t);
    uint8_t ol = C(OUT), wood = C(WOOD), woodD = C(WOOD_D), woodL = C(WOOD_L);
    fr.rect(DOOR_X0 - 3, DOOR_Y0 - 3, DOOR_X1 - DOOR_X0 + 7, FLOOR_Y - DOOR_Y0 + 3, ol);
    fr.rect(DOOR_X0 - 2, DOOR_Y0 - 2, DOOR_X1 - DOOR_X0 + 5, FLOOR_Y - DOOR_Y0 + 2, woodD);
    // how far open (0 shut .. 1 wide open) and the shake
    float open = 0; int shake = 0;
    if (pf < DOOR_SHUT) open = 1;                                         // mom saying goodnight
    else if (pf < DOOR_SHUT + 5) open = 1 - (pf - DOOR_SHUT) / 5.0f;
    if (pf >= G0 && pf < JB + 4) open = std::min(0.3f, (pf - G0) / 10.0f);   // creaks open: the goo comes in under it
    if (pf >= JB + 4 && pf < JB + 8) open = 0.3f * (1 - (pf - JB - 4) / 4.0f);
    if (endT_ >= 0 && ee >= 0) {
        for (int s : { 0, SHAKE2, SHAKE3 }) if (ee >= s && ee < s + 5) shake = (ee % 2) ? 2 : -2;   // bang bang bang
        if (ee >= DOOR_IN && ee < ALL_GONE) open = std::min(1.0f, (ee - DOOR_IN) / 4.0f);
        else if (ee >= ALL_GONE && ee < ALL_GONE + 5) open = 1 - (ee - ALL_GONE) / 5.0f;
    }
    if (open > 0) {                                                       // warm hallway light behind it
        fr.rect(DOOR_X0, DOOR_Y0, DOOR_X1 - DOOR_X0 + 1, FLOOR_Y - DOOR_Y0, C(endT_ >= 0 ? RGB{ 250, 220, 150 } : RGB{ 30, 26, 40 }));
        if (pf < DOOR_SHUT + 5 && endT_ < 0) fr.rect(DOOR_X0, DOOR_Y0, DOOR_X1 - DOOR_X0 + 1, FLOOR_Y - DOOR_Y0, C(250, 220, 150));
        int w = rr((DOOR_X1 - DOOR_X0) * (1 - open * 0.85f));             // the door swings into the room, to the right
        fr.rect(DOOR_X1 - w, DOOR_Y0, w + 1, FLOOR_Y - DOOR_Y0, wood); fr.vline(DOOR_X1 - w, DOOR_Y0, FLOOR_Y - 1, ol);
    } else {
        int x0 = DOOR_X0 + shake;
        fr.rect(x0, DOOR_Y0, DOOR_X1 - DOOR_X0 + 1, FLOOR_Y - DOOR_Y0, wood);
        fr.rect(x0 + 5, DOOR_Y0 + 6, DOOR_X1 - DOOR_X0 - 9, 28, woodD); fr.rect(x0 + 6, DOOR_Y0 + 7, DOOR_X1 - DOOR_X0 - 11, 26, woodL);
        fr.rect(x0 + 5, DOOR_Y0 + 40, DOOR_X1 - DOOR_X0 - 9, 32, woodD); fr.rect(x0 + 6, DOOR_Y0 + 41, DOOR_X1 - DOOR_X0 - 11, 30, woodL);
        disc(fr, x0 + DOOR_X1 - DOOR_X0 - 4, DOOR_Y0 + 38, 1.5f, C(236, 200, 110));
        if (shake) for (int k = 0; k < 3; ++k) { fr.line(DOOR_X0 - 8, DOOR_Y0 + 20 + k * 12, DOOR_X0 - 4, DOOR_Y0 + 22 + k * 12, C(255, 240, 200)); fr.line(DOOR_X1 + 4, DOOR_Y0 + 22 + k * 12, DOOR_X1 + 8, DOOR_Y0 + 20 + k * 12, C(255, 240, 200)); }
    }
}

bool BedroomRound::wolfOut(int t) const { return closT_ >= 0 && t >= closT_ && endT_ < 0; }

// the closet on the right wall; the werewolf leans out of it (7 lines) and is gone when the ending starts
void BedroomRound::drawCloset(Frame& fr, int t) {
    float open = 0;
    if (wolfOut(t)) open = std::min(1.0f, (t - closT_) / 6.0f);
    int of = std::max(0, std::min(4, rr(open * 4)));
    blitFrame(fr, BedroomArt::CLOSET, 90, of, 368, 9);
    bool wolf = wolfOut(t);
    if (wolf && of >= 3) {
        int k = (t / 2) % 8;
        blitFrame(fr, BedroomArt::WOLF_IN, 71, t % 50 < 2 ? 8 : k, 368, 46, 100000, &BedroomArt::CLOSET, 90, of, 368, 9, BedroomArt::CLOSET_INSIDE);
        if (of == 4) {
            if (t % 50 >= 2) blitFrame(fr, BedroomArt::WOLF_HEAD, 57, k, 368, 46);
            blitFrame(fr, BedroomArt::WOLF_CLAWS, 20, (t / 3) % 4, 411, 80);
        }
    }
}

float BedroomRound::tentOut(int t) const {
    if (chestT_ < 0 || t < chestT_) return 0;
    float k = ease((t - chestT_ - 2) / 8.0f);
    if (endT_ >= 0 && e(t) >= TENT_HIDE) k *= 1 - ease((e(t) - TENT_HIDE) / 6.0f);   // they slither back
    return k;
}

// the toy chest on the left; its lid bursts open at 9 lines
void BedroomRound::drawChest(Frame& fr, int t) {
    uint8_t ol = C(OUT), wood = C(150, 96, 60), woodD = C(110, 70, 46), woodL = C(184, 128, 80), band = C(196, 160, 80), dark = C(14, 10, 22);
    float lid = chestT_ >= 0 && t >= chestT_ ? std::min(1.0f, (t - chestT_) / 3.0f) : 0;
    if (endT_ >= 0 && e(t) >= TENT_HIDE + 4) lid = std::max(0.0f, 1 - (e(t) - TENT_HIDE - 4) / 3.0f);
    auto P = [&](int i, float dy = 0) { return std::make_pair(CHEST[i][0], CHEST[i][1] + dy); };
    // side face (bn, fn, tfn, tbn), front face (fn, ff, tff, tfn)
    quad(fr, CHEST[0][0], CHEST[0][1], CHEST[2][0], CHEST[2][1], CHEST[6][0], CHEST[6][1], CHEST[4][0], CHEST[4][1], woodD);
    quad(fr, CHEST[2][0], CHEST[2][1], CHEST[3][0], CHEST[3][1], CHEST[7][0], CHEST[7][1], CHEST[6][0], CHEST[6][1], wood);
    for (float s : { 0.3f, 0.7f }) fr.line(rr(lerp(CHEST[2][0], CHEST[3][0], s)), rr(lerp(CHEST[2][1], CHEST[3][1], s)), rr(lerp(CHEST[6][0], CHEST[7][0], s)), rr(lerp(CHEST[6][1], CHEST[7][1], s)), band);
    fr.line(rr(CHEST[2][0]), rr(CHEST[2][1]), rr(CHEST[6][0]), rr(CHEST[6][1]), ol);
    // the top: shut, or the dark mouth with the lid flipped up behind it
    if (lid <= 0) {
        quad(fr, CHEST[4][0], CHEST[4][1], CHEST[6][0], CHEST[6][1], CHEST[7][0], CHEST[7][1], CHEST[5][0], CHEST[5][1], woodL);
    } else {
        quad(fr, CHEST[4][0], CHEST[4][1], CHEST[6][0], CHEST[6][1], CHEST[7][0], CHEST[7][1], CHEST[5][0], CHEST[5][1], dark);
        float up = 34 * lid;                                              // the lid stands up on its back edge
        quad(fr, CHEST[4][0], CHEST[4][1], CHEST[5][0], CHEST[5][1], CHEST[5][0] + 4 * lid, CHEST[5][1] - up, CHEST[4][0] + 4 * lid, CHEST[4][1] - up, woodL);
        fr.line(rr(CHEST[4][0]), rr(CHEST[4][1]), rr(CHEST[5][0]), rr(CHEST[5][1]), ol);
    }
    (void)P;
    fr.line(rr(CHEST[4][0]), rr(CHEST[4][1]), rr(CHEST[6][0]), rr(CHEST[6][1]), ol); fr.line(rr(CHEST[6][0]), rr(CHEST[6][1]), rr(CHEST[7][0]), rr(CHEST[7][1]), ol);
    fr.line(rr(CHEST[0][0]), rr(CHEST[0][1]), rr(CHEST[2][0]), rr(CHEST[2][1]), ol); fr.line(rr(CHEST[2][0]), rr(CHEST[2][1]), rr(CHEST[3][0]), rr(CHEST[3][1]), ol);
}

// the hook with his jacket and red hoodie; at 3 lines an eye flies in and settles in the hood
void BedroomRound::drawHoodie(Frame& fr, int t) {
    uint8_t ol = C(OUT);
    fr.rect(206, 56, 66, 3, C(WOOD_D)); fr.hline(206, 271, 56, C(WOOD_L));
    fr.poly({ {213, 59}, {229, 59}, {232, 100}, {210, 100} }, ol); fr.poly({ {214, 60}, {228, 60}, {231, 99}, {211, 99} }, C(110, 84, 64));   // jacket
    fr.vline(221, 62, 98, C(86, 64, 50));
    int ee = e(t);
    bool eye = eyeT_ >= 0 && t >= eyeT_ + 14 && !(endT_ >= 0 && ee >= GOO_DRY + 4);
    int lift = eye ? rr(4 + std::sin(t * 0.3f) * 2) : 0;                 // the hoodie floats up off its hook
    if (endT_ >= 0 && ee >= GOO_DRY && ee < GOO_DRY + 4) lift = rr(4 * (1 - (ee - GOO_DRY) / 4.0f));
    int y = 58 - lift;
    uint8_t red = C(196, 54, 60), redD = C(146, 36, 46), redL = C(226, 96, 96);
    fr.poly({ {238, y + 2}, {264, y + 2}, {268, y + 44}, {234, y + 44} }, ol);
    fr.poly({ {239, y + 3}, {263, y + 3}, {267, y + 43}, {235, y + 43} }, red);
    fr.ellipse(HOOD_X, y + 13, 10, 10, ol); fr.ellipse(HOOD_X, y + 13, 9, 9, red);   // the hood
    fr.ellipse(HOOD_X, y + 14, 6, 6, C(40, 14, 22));                     // its dark mouth
    fr.line(236, y + 43, 236, y + 10, redD); fr.vline(259, y + 22, y + 42, redD); fr.hline(244, 258, y + 32, redL);   // pocket
    fr.rect(243, y + 20, 2, 10, C(240, 230, 220)); fr.rect(257, y + 20, 2, 10, C(240, 230, 220));   // drawstrings
    if (eye) {                                                            // a huge eye in the hood, looking about
        int lk = rr(std::sin(t * 0.13f) * 2), ey = y + 14;
        bool blink = t % 60 < 2;
        if (blink) fr.hline(HOOD_X - 5, HOOD_X + 5, ey, C(250, 240, 230));
        else {
            fr.ellipse(HOOD_X, ey, 5, 4, C(250, 240, 230));
            disc(fr, HOOD_X + lk, ey, 2.5f, C(200, 30, 40)); disc(fr, HOOD_X + lk, ey, 1, C(10, 6, 10));
            fr.set(HOOD_X + lk - 1, ey - 1, C(255, 255, 255));
            for (int k = -4; k <= 4; k += 4) fr.set(HOOD_X + k, ey - 3, C(200, 120, 120));   // red veins
        }
    }
    // the eye flying in from the window to the hood
    if (eyeT_ >= 0 && t >= eyeT_ && t < eyeT_ + 14) {
        float u = ease((t - eyeT_) / 14.0f);
        int x = rr(lerp(60, HOOD_X, u)), yy = rr(lerp(70, y + 14, u) - std::sin(u * PI) * 30);
        fr.ellipse(x, yy, 4, 3, C(250, 240, 230)); disc(fr, x + 1, yy, 1.5f, C(200, 30, 40));
        fr.line(x - 5, yy, x - 9, yy - 2, C(200, 30, 40));
    }
}

// the bed: foot far away (top), head near us (bottom); he lies with the back of his head toward us
void BedroomRound::drawBed(Frame& fr, int t) {
    int ee = e(t), pf = f(t);
    float shake = 0;
    int eyeLand = eyeT_ >= 0 ? eyeT_ + 14 : -1000;
    if ((eyeT_ >= 0 && t >= eyeLand && t < eyeLand + 22) || (closT_ >= 0 && t >= closT_ && t < closT_ + 16) || (chestT_ >= 0 && t >= chestT_ && t < chestT_ + 18)
        || (pf >= L0 && pf < L0 + 16)) shake = (t % 2) ? 1 : -1;
    blitFrame(fr, BedroomArt::BED_FRAME, 131, 0, 175, 167);
    bool hide = snatchT_ >= 0 && t - snatchT_ < 10;
    int k;
    if (endT_ >= 0 && ee >= 0) {
        bool banging = false;
        for (int s : { 0, SHAKE2, SHAKE3 }) if (ee >= s && ee < s + 6) banging = true;
        if (ee >= KID_UP) k = -1;
        else if (banging) k = 7 + (ee % 2);
        else if (ee >= DOOR_IN + 4) k = 5;
        else k = 6;
        shake = 0;
    } else if (pf < MOM_LEAVE + 4) k = 4;
    else if (pf < L0) k = (pf / 8) % 2;
    else if (pf < PLAY0) { static const int LOOK[4] = { 2, 3, 4, 3 }; k = LOOK[(pf / 6) % 4]; }
    else if (hide) k = 6;
    else { int g = (pf / 7) % 9; k = g == 3 ? 2 : g == 4 ? 3 : g == 6 ? 4 : 5; }
    if (k >= 0) blitFrame(fr, BedroomArt::KID_BED, 122, k, 179 + rr(shake), 175);
    else if (ee < CRAWL1) blitFrame(fr, BedroomArt::KID_CRAWL, 118, std::min(7, (ee - KID_UP) / 2), 181, 175);
    else blitFrame(fr, BedroomArt::KID_BED, 122, 10, 179, 175);
    bool lampOn = pf < L0 && endT_ < 0;
    if (lampOn) fr.ditherEllipse(321, 214, 30, 14, C(150, 120, 90));
    blitFrame(fr, BedroomArt::LAMP_TABLE, 44, lampOn ? 0 : 1, 299, 204);
    blitFrame(fr, BedroomArt::HEADBOARD, 131, 0, 175, 223);
}

void BedroomRound::drawToys(Frame& fr) {
    // blocks, a ball and a teddy by the Memory Book
    static const RGB B[3] = { {220, 70, 70}, {70, 120, 210}, {240, 200, 70} };
    for (int k = 0; k < 3; ++k) {
        int x = 92 + k * 9, y = 250;
        fr.rect(x, y, 9, 9, C(OUT)); fr.rect(x + 1, y + 1, 7, 7, C(B[k])); text(fr, std::string(1, (char)('A' + k)), x + 3, y + 2, C(255, 255, 255));
    }
    disc(fr, 126, 255, 4, C(OUT)); disc(fr, 126, 255, 3, C(230, 90, 90)); fr.hline(123, 129, 255, C(250, 250, 250));
    fr.rect(134, 251, 16, 6, C(OUT)); fr.rect(135, 252, 14, 4, C(80, 160, 90)); disc(fr, 138, 258, 2, C(OUT)); disc(fr, 146, 258, 2, C(OUT));
    uint8_t ol = C(OUT), fur = C(150, 100, 60), furL = C(196, 146, 94);   // teddy
    disc(fr, 74, 248, 7, ol); disc(fr, 74, 248, 6, fur); disc(fr, 74, 237, 6, ol); disc(fr, 74, 237, 5, fur);
    disc(fr, 69, 232, 2.5f, ol); disc(fr, 79, 232, 2.5f, ol); disc(fr, 69, 232, 1.5f, fur); disc(fr, 79, 232, 1.5f, fur);
    fr.ellipse(74, 239, 2, 1, furL); fr.set(72, 236, ol); fr.set(76, 236, ol); fr.set(74, 239, ol); fr.ellipse(74, 250, 3, 3, furL);
}

// toy soldiers on the floor: one per line; the new ones hop in, then salute
void BedroomRound::drawSoldiers(Frame& fr, int t) {
    int n = std::min(20, lines_), newAt = t - clearT_;
    std::vector<int> order;
    for (int i = 0; i < n; ++i) order.push_back(i);
    std::stable_sort(order.begin(), order.end(), [](int a, int b) { return SOLDIER_SPOT[a][1] < SOLDIER_SPOT[b][1]; });
    for (int i : order) {
        int x = SOLDIER_SPOT[i][0], y = SOLDIER_SPOT[i][1], hop = 0;
        bool fresh = newAt >= 0 && newAt < 12 && i >= n - clearN_;
        if (fresh && newAt < 5) hop = rr(std::sin(newAt / 5.0f * PI) * 8);
        fr.ellipse(x, y + 1, 4, 1, C(70, 44, 36));
        blit(fr, SOLDIER, x - 3, y - 13 - hop);
        if (fresh && newAt >= 6) { fr.line(x + 3, y - 5, x + 3, y - 9, C(200, 50, 56)); fr.set(x + 2, y - 9, C(246, 204, 164)); }   // salute
        else fr.vline(x + 3, y - 6, y - 4, C(200, 50, 56));
    }
}

// the book the goo swallowed, wrapped up on the rug, heaving
void BedroomRound::drawCocoon(Frame& fr, int t) {
    uint8_t g0 = C(GOO0), g2 = C(GOO2), g3 = C(GOO3);
    float heave = std::sin(t * 0.25f) * 1.2f;
    fr.ellipse(COCOON_X, COCOON_Y - 3, 14 + rr(heave), 7 - rr(heave / 2), g0);
    fr.rect(COCOON_X + 6, COCOON_Y - 8, 6, 4, C(250, 230, 120)); fr.rect(COCOON_X + 6, COCOON_Y - 8, 6, 1, C(OUT));   // a corner of the book
    for (int k = -10; k <= 10; k += 5) fr.set(COCOON_X + k, COCOON_Y - 8 + std::abs(k) / 5, g3);
    for (int k = 0; k < 4; ++k) { int x = COCOON_X - 12 + k * 8; fr.vline(x, COCOON_Y + 1, COCOON_Y + 2 + (k + t / 4) % 2, g0); }
    fr.set(COCOON_X - 6, COCOON_Y - 6, g2);
}

// goo hanging over the top of the board: 2 rows deep across the top, running down the sides and thinning out
void BedroomRound::drawBoardGoo(Frame& fr, int t) {
    int pf = f(t), ee = e(t);
    if (pf < S0) return;
    float spread = std::min(1.0f, (pf - S0 + 1) / 6.0f);
    int keep = dry(t) ? rr(16 * (1 - ease((ee - GOO_DRY) / 12.0f))) : 16;
    if (keep <= 0) return;
    uint8_t g0 = C(GOO0), g2 = C(GOO2), g3 = C(GOO3);
    for (int x = BX - 3; x < BX + 83; ++x) {
        float edge = std::min(x - (BX - 3), BX + 83 - x) / 6.0f;             // 0 at the sides
        int col = (x - BX) / 8;
        int depth = 16 + rr(std::sin(x * 0.7f) * 2 + std::sin(x * 0.23f + t * 0.1f) * 2);   // 2 rows, lumpy
        if (x < BX + 10 || x > BX + 69) depth = 16 + rr(26 * (1 - std::min(1.0f, edge)) + 8);   // down the sides, thinner at the bottom
        depth = rr(depth * spread);
        int drip = ((x * 37) % 11 == 0) ? rr(4 + 3 * std::sin(t * 0.2f + x)) : 0;            // drops stretching down
        (void)col;
        for (int y = BY - 3; y < BY + depth + drip; ++y) {
            bool side = (x < BX + 10 || x > BX + 69) && y > BY + 18;
            if (side) { int w = rr(10 - (y - BY - 18) * 0.35f); if (w <= 0) continue; if (x >= BX + w && x <= BX + 79 - w) continue; }
            if (BAYER[y & 3][x & 3] >= keep) continue;
            uint8_t c = g0;
            if (y == BY + depth - 2 && x % 5 == 0) c = g2;
            if (y < BY + 1 && x % 7 < 3) c = g3;
            fr.set(x, y, c);
        }
    }
    // eyes blink in the goo now and then
    if (!dry(t)) for (int k = 0; k < 2; ++k) { int x = BX + 20 + k * 36, y = BY + 6; if ((t / 13 + k) % 7 == 0) { fr.hline(x, x + 2, y, C(200, 30, 40)); } }
}

// werewolf stains inside the board (over the blocks)
void BedroomRound::drawStains(Frame& fr, int t) {
    for (auto& s : stains_) {
        float age = (t - s.born) / 70.0f;
        int keep = age > 0.75f ? (int)(16 * (1 - (age - 0.75f) * 4)) : 16;
        uint8_t c = C(20, 10, 30), sp = C(70, 40, 90), paw = C(60, 30, 70);
        float mx = 0, my = 0;
        for (auto& q : s.cells) {
            int x = BX + q.first * 8, y = BY + q.second * 8;
            for (int yy = y; yy < y + 8; ++yy) for (int xx = x; xx < x + 8; ++xx) if (BAYER[yy & 3][xx & 3] < keep) fr.set(xx, yy, c);
            fr.set(x + 1 + (t + q.first) % 5, y + 2 + q.second % 4, sp);
            mx += q.first; my += q.second;
        }
        if (!s.cells.empty() && keep > 4) {
            mx /= s.cells.size(); my /= s.cells.size();
            float px = BX + mx * 8 + 4, py = BY + my * 8 + 4;
            fr.ellipse(rr(px), rr(py + 1.5f), 4, 3, paw);
            for (int k = 0; k < 4; ++k) fr.ellipse(rr(px - 5 + k * 4), rr(py - 5.5f), 1, 1, paw);
        }
    }
}

// ---------------------------------------------------------------- black tentacles
void BedroomRound::drawTentacle(Frame& fr, float bx, float by, float tx, float ty, int t, float w0, float w1, float curl, int seed) {
    const int n = 26;
    float ph = hash01((unsigned)seed * 7 + 3) * 6.28f;
    std::vector<std::pair<float, float>> pts;
    for (int i = 0; i <= n; ++i) {
        float k = (float)i / n, x = bx + (tx - bx) * k, y = by + (ty - by) * k;
        float nx = -(ty - by), ny = (tx - bx), L = std::hypot(nx, ny); if (L == 0) L = 1;
        float wav = std::sin(k * 5 + t * 0.35f + ph) * 6 * k;
        pts.push_back({ x + nx / L * wav, y + ny / L * wav });
    }
    if (curl > 0) {
        float lx = pts.back().first, ly = pts.back().second;
        for (int j = 1; j < 6; ++j) { float a = j * 0.6f * curl; pts.push_back({ lx + std::sin(a) * 4, ly - std::cos(a) * 4 + 4 }); }
    }
    uint8_t g0 = C(GOO0), g2 = C(GOO2), g3 = C(GOO3), suck = C(96, 64, 110);
    for (size_t i = 0; i < pts.size(); ++i) disc(fr, pts[i].first, pts[i].second, (w0 + (w1 - w0) * std::min(1.0f, (float)i / n)) / 2, g0);
    for (size_t i = 0; i < pts.size(); ++i) {
        float r = (w0 + (w1 - w0) * std::min(1.0f, (float)i / n)) / 2, x = pts[i].first, y = pts[i].second;
        if (r > 1.4f) fr.set(rr(x - r * 0.5f), rr(y - r * 0.8f), i % 3 == 0 ? g3 : g2);
        if (i % 3 == 1 && r > 1.8f && (int)i < n) fr.set(rr(x + r * 0.5f), rr(y + r * 0.6f), suck);
    }
}

void BedroomRound::drawTentacles(Frame& fr, int t) {
    float tk = tentOut(t);
    if (tk <= 0) return;
    float prog = 0, gx = 0, gy = 0;
    if (snatchT_ >= 0) {                                                  // one reaches for the falling piece
        int g = t - snatchT_;
        prog = g < 7 ? ease(g / 5.0f) : 1 - ease((g - 7) / 6.0f);
        gx = (float)(pieceX_ + 8); gy = (float)(std::max(pieceY_, BY + 44) + 8);
    }
    // out of the chest
    float tfx = CHEST[6][0], tfy = CHEST[6][1], tgx = CHEST[7][0], tgy = CHEST[7][1];
    for (int j = 0; j < 4; ++j) {
        float s = 0.15f + j * 0.24f, x = tfx + (tgx - tfx) * s - 4, y = tfy + (tgy - tfy) * s - 2, L = (18 + (j % 2) * 10) * tk;
        drawTentacle(fr, x, y + 2, x + std::sin(t * 0.25f + j) * 8, y - L, t, 7, 2, 1.0f, 40 + j);
    }
    if (snatchT_ >= 0 && snatchSrc_ == 0) {
        float bx = (tfx + tgx) / 2, by = (tfy + tgy) / 2;
        drawTentacle(fr, bx, by, bx + (gx - bx) * prog, by + (gy - by) * prog, t, 8, 3, prog > 0.9f ? 1.0f : 0.3f, 88);
    }
}

void BedroomRound::drawMist(Frame& fr, int t, float k) {
    static const float SRC[3][3] = { {60,156,40}, {176,230,24}, {300,230,24} };
    uint8_t m = C(14, 8, 22), ml = C(70, 50, 100);
    for (int si = 0; si < 3; ++si)
        for (int j = 0; j < 14; ++j) {
            unsigned id = si * 14 + j;
            float a = hash01(id * 4), b = hash01(id * 4 + 1), dir = hash01(id * 4 + 2) < 0.5f ? -1.0f : 1.0f, d = hash01(id * 4 + 3);
            float life = std::fmod(t * 0.6f + j * 7 + a * 20, 40.0f) / 40.0f;
            float x = SRC[si][0] + (b - 0.5f) * SRC[si][2] + std::sin(life * 6 + j) * 6 + life * dir * 18;
            float y = SRC[si][1] - life * 18 + d * 4, r = 3 + life * 8;
            int keep = (int)(130 * (1 - life) * k / 255.0f * 16);
            for (int py = (int)(y - r * 0.6f); py <= (int)(y + r * 0.6f); ++py)
                for (int px = (int)(x - r); px <= (int)(x + r); ++px) {
                    float u = (px - x) / r, v = (py - y) / (r * 0.6f);
                    if (u * u + v * v <= 1 && BAYER[py & 3][px & 3] < keep) fr.set(px, py, m);
                }
            if (life < 0.3f) fr.set(rr(x - r * 0.4f), rr(y - r * 0.3f), ml);
        }
}

void BedroomRound::drawMoonPatch(Frame& fr, float k) {
    static const float P[4][2] = { {118,150}, {196,136}, {248,168}, {150,196} };
    float lvl = 12 * k;
    for (int y = 134; y < 198; ++y)
        for (int x = 116; x < 250; ++x) {
            if (!inPoly(P, 4, x + 0.5f, y + 0.5f) || BAYER[y % 4][x % 4] >= lvl) continue;
            if (std::fabs((x - 118) * 0.55f - (y - 150) * 0.9f - 30) < 1.2f || std::fabs(x - (150 + (y - 150) * 0.8f)) < 0.9f) continue;   // window bars
            RGB c = palette()[fr.get(x, y)];
            fr.set(x, y, C(std::min(255, c.r + 26), std::min(255, c.g + 30), std::min(255, c.b + 44)));
        }
}

// ---------------------------------------------------------------- the intro: goodnight, lightning, the book, the goo
void BedroomRound::drawIntro(Frame& fr, int pf) {
    uint8_t g0 = C(GOO0), g3 = C(GOO3);
    if (pf < MOM_LEAVE + 4) {                                             // mom in the doorway: goodnight
        bool leaving = pf >= MOM_LEAVE;
        People::drawMom(fr, 297, FLOOR_Y, leaving ? People::BACK : People::FRONT, leaving ? pf % 4 : -1, pf >= MOM_TALK0 && pf < MOM_TALK1 && pf % 4 < 2);
        if (pf >= MOM_TALK0 && pf < MOM_TALK1) bubble(fr, 297, 54, "GOOD NIGHT,", "SWEETIE.", 297, 84);
    }
    // the NEXT book flies off the shelf to hang over the board
    auto bookAt = [&](float& x, float& y) {
        if (pf < BOOK_FLY1) { float u = ease((pf - BOOK_FLY0) / 16.0f); x = lerp(186, 136, u); y = lerp(28, 62, u) - std::sin(u * PI) * 20; }
        else if (pf < JB) { x = 136 + std::sin(pf * 0.3f) * 2; y = 62 + std::sin(pf * 0.2f) * 2; }
        else if (pf < FIGHT1) {                                           // the goo jumps on it; it fights in the air
            float u = clamp01((pf - JB) / 4.0f);
            x = lerp(136, 160, u) + ((pf % 2) ? 3 : -3); y = lerp(62, 70, u) + ((pf % 3) - 1) * 2;
        } else { float u = clamp01((pf - FIGHT1) / 7.0f); x = lerp(160, COCOON_X, u); y = lerp(70, COCOON_Y - 6, u * u); }
    };
    if (pf >= BOOK_FLY0 && pf < DROP1) {
        float x, y; bookAt(x, y);
        int bx = rr(x), by = rr(y), flap = (pf / 2) % 2;
        fr.poly({ {bx - 10, by - 2 - flap * 3}, {bx, by}, {bx, by + 8}, {bx - 10, by + 6 - flap * 3} }, C(OUT));
        fr.poly({ {bx + 10, by - 2 - flap * 3}, {bx, by}, {bx, by + 8}, {bx + 10, by + 6 - flap * 3} }, C(OUT));
        fr.poly({ {bx - 9, by - 1 - flap * 3}, {bx, by + 1}, {bx, by + 7}, {bx - 9, by + 5 - flap * 3} }, C(250, 230, 120));
        fr.poly({ {bx + 9, by - 1 - flap * 3}, {bx, by + 1}, {bx, by + 7}, {bx + 9, by + 5 - flap * 3} }, C(255, 250, 230));
        if (pf >= BOOK_FLY1 && pf < JB) text(fr, "NEXT", bx - 7, by - 12, C(255, 250, 200));
        if (pf >= JB) {                                                   // goo wrapped round it
            for (int k = 0; k < 6; ++k) disc(fr, bx - 8 + k * 3 + (pf + k) % 3 - 1, by + 2 + (k % 2) * 3, 3.0f, g0);
            fr.set(bx - 3, by + 1, g3);
        }
    }
    // goo 1: out under the door, across the floor, then it leaps at the book
    if (pf >= GOO1_0 && pf < JB + 2) {
        static const float PATH[4][2] = { {300,123}, {276,130}, {240,136}, {206,142} };
        float u = clamp01((float)(pf - GOO1_0) / (GOO1_1 - GOO1_0)) * 3;
        int i = std::min(2, (int)u); float k = u - i;
        float x = lerp(PATH[i][0], PATH[i + 1][0], k), y = lerp(PATH[i][1], PATH[i + 1][1], k);
        if (pf >= GOO1_1) { float j = (pf - GOO1_1) / 4.0f; x = lerp(206, 140, j); y = lerp(142, 64, j) - std::sin(j * PI) * 20; }
        for (int s = 0; s < 5; ++s) disc(fr, x + s * 5, y + (pf >= GOO1_1 ? 0 : 1), 3.5f - s * 0.5f, g0);   // a crawling blob with a trail
        fr.set(rr(x) - 1, rr(y) - 2, C(200, 30, 40)); fr.set(rr(x) + 1, rr(y) - 2, C(200, 30, 40));
    }
    // goo 2: in over the window sill, then it leaps onto the board
    if (pf >= WG0 && pf < S0) {
        static const float PATH[4][2] = { {30,112}, {52,110}, {70,107}, {84,105} };
        float x, y;
        if (pf < GOO2_1) { float u = (float)(pf - WG0) / (GOO2_1 - WG0) * 3; int i = std::min(2, (int)u); float k = u - i; x = lerp(PATH[i][0], PATH[i + 1][0], k); y = lerp(PATH[i][1], PATH[i + 1][1], k); }
        else { float j = (pf - GOO2_1) / 7.0f; x = lerp(84, 136, j); y = lerp(105, 80, j) - std::sin(j * PI) * 16; }
        for (int s = 0; s < 6; ++s) disc(fr, x - s * 5, y + s * 0.3f, 4.5f - s * 0.6f, g0);
        fr.set(rr(x) + 1, rr(y) - 2, C(200, 30, 40)); fr.set(rr(x) + 3, rr(y) - 2, C(200, 30, 40));
    }
}

// ---------------------------------------------------------------- the ending: parents come, the kid walks out with them
void BedroomRound::drawEnding(Frame& fr, int t) {
    int ee = e(t);
    if (ee >= DOOR_IN + 2 && ee < ALL_GONE) {
        People::drawMom(fr, 289, FLOOR_Y, People::FRONT, -1, ee >= TALK1 && ee < TALK1 + TALK_LEN && ee % 4 < 2);
        blit(fr, DAD_BODY, 299, FLOOR_Y - 27); blit(fr, DAD_HEAD, 299, FLOOR_Y - 42);
        if (ee >= TALK3 && ee < TALK3 + TALK_LEN && ee % 4 < 2) fr.rect(306, FLOOR_Y - 32, 2, 1, C(176, 80, 84));
    }
    if (ee >= TALK1 && ee < TALK1 + TALK_LEN) bubble(fr, 260, 30, "ARE YOU", "OKAY?", 286, 86);
    if (ee >= TALK2 && ee < TALK2 + TALK_LEN) bubble(fr, 240, 196, "I'M SCARED...", "", 240, 220);
    if (ee >= TALK3 && ee < TALK3 + TALK_LEN) bubble(fr, 340, 30, "COME SLEEP", "WITH US.", 308, 84);
    if (ee < CRAWL1 || ee >= ALL_GONE) return;
    auto kid = [&](const Sprite& sp, int k, float x, float y, int clipY = 100000) { blitFrame(fr, sp, 32, k, rr(x) - 16, rr(y) - 44, clipY); };
    if (ee < RISE1) {
        float u = ease((ee - CRAWL1 + 1) / (float)(RISE1 - CRAWL1));
        kid(BedroomArt::PJ_WALK, 0, 240, 182 + (1 - u) * 36, 182);
    } else if (ee < HOP1) {
        int h = ee - RISE1;
        if (h < 2) kid(BedroomArt::PJ_WALK, 0, 240, 184);
        else { float u = (h - 2) / (float)(HOP1 - RISE1 - 2); kid(BedroomArt::PJ_WALK, 0, lerp(240, 256, u), lerp(182, 163, u) - std::sin(u * PI) * 16); }
    } else if (ee < LOOK1) kid(BedroomArt::PJ_TURN, ee < HOP1 + 1 ? 2 : 0, 256, ee == HOP1 ? 165 : 163);
    else if (ee < KID_WALK0) kid(BedroomArt::PJ_TURN, 1, 256, 163);
    else {
        float x, y; int w = ee - KID_WALK0;
        static const float PATH[3][2] = { {256, 163}, {278, 166}, {300, FLOOR_Y + 3} };
        float L0p = 22.2f, L1p = 46.0f, d = std::min(L0p + L1p, (float)w * (L0p + L1p) / (KID_DOOR - KID_WALK0));
        if (d < L0p) { float u = d / L0p; x = lerp(PATH[0][0], PATH[1][0], u); y = lerp(PATH[0][1], PATH[1][1], u); }
        else { float u = (d - L0p) / L1p; x = lerp(PATH[1][0], PATH[2][0], u); y = lerp(PATH[1][1], PATH[2][1], u); }
        bool moving = ee < KID_DOOR;
        int bob = moving && (w / 2) % 2;
        if (d < L0p && moving) kid(BedroomArt::PJ_TURN, 1, x, y - bob);
        else kid(BedroomArt::PJ_WALK, moving ? 1 + (w / 2) % 4 : 0, x, y - bob);
    }
}

// ---------------------------------------------------------------- frame drawing
void BedroomRound::drawBackground(Frame& fr, const Tetris&, int t) {
    int pf = f(t), ee = e(t);
    drawRoom(fr);
    drawWindow(fr, endT_ >= 0 ? PLAY0 + 100 + ee : pf, ee);
    float moon = pf >= G0 ? ease((pf - G0) / 12.0f) : 0.0f;
    if (moon > 0.4f) drawMoonPatch(fr, (moon - 0.4f) / 0.6f * (dry(t) ? 1 - ease((ee - GOO_DRY) / 20.0f) * 0.5f : 1.0f));
    drawShelf(fr, pf);
    drawDoor(fr, t);
    drawCloset(fr, t);
    drawHoodie(fr, t);
    drawCommonUI(fr);
}

void BedroomRound::drawForeground(Frame& fr, const Tetris&, int t) {
    int pf = f(t), ee = e(t);
    drawStains(fr, t);
    drawBoardGoo(fr, t);
    // the floor, back to front: chest + its tentacles, the cocoon, soldiers, tentacles under the bed, the bed, toys
    drawChest(fr, t);
    drawTentacles(fr, t);
    if (pf >= DROP1) drawCocoon(fr, t);
    drawSoldiers(fr, t);
    float tk = tentOut(t);
    if (tk > 0) {                                                         // from under the bed (drawn first: they come out from UNDER it)
        for (int side = -1; side <= 1; side += 2)
            for (int j = 0; j < 3; ++j) {
                float tq = 0.25f + j * 0.28f;
                float fx = (float)(side < 0 ? BED_FL[0] : BED_FR[0]), hx = (float)(side < 0 ? BED_HL[0] : BED_HR[0]);
                float y = BED_FL[1] + (BED_HL[1] - BED_FL[1]) * tq, x = fx + (hx - fx) * tq + side * 2, L = (28 + j * 8) * tk;
                drawTentacle(fr, x, y + 3, x + side * L, y + 8 + j * 3 + std::sin(t * 0.3f + j) * 5, t, 9, 2, 1.0f, j + (side > 0 ? 10 : 0));
            }
        if (snatchT_ >= 0 && snatchSrc_ == 1) {
            int g = t - snatchT_;
            float prog = g < 7 ? ease(g / 5.0f) : 1 - ease((g - 7) / 6.0f), gx = (float)(pieceX_ + 8), gy = (float)(std::max(pieceY_, BY + 44) + 8);
            float bx = BED_FL[0] + (BED_HL[0] - BED_FL[0]) * 0.4f, by = BED_FL[1] + (BED_HL[1] - BED_FL[1]) * 0.4f;
            drawTentacle(fr, bx, by, bx + (gx - bx) * prog, by + (gy - by) * prog, t, 8, 3, prog > 0.9f ? 1.0f : 0.3f, 77);
        }
    }
    drawBed(fr, t);
    drawToys(fr);
    if (tk > 0) drawMist(fr, t, tk);
    if (pf < PLAY0) drawIntro(fr, pf);
    if (endT_ >= 0 && ee >= 0) drawEnding(fr, t);
}

void BedroomRound::drawHud(Frame& fr, int, int t) {
    (void)t;
    if (swapLeftRight()) {                                                // while left and right are swapped
        int x = BX + 26, y = BY - 24;
        uint8_t bg = C(60, 16, 26), ol = C(200, 60, 70), ar = C(250, 210, 210), lt = C(255, 200, 90);
        fr.rect(x, y, 31, 12, ol); fr.rect(x + 1, y + 1, 29, 10, bg);
        fr.set(x, y, bg); fr.set(x + 30, y, bg); fr.set(x, y + 11, bg); fr.set(x + 30, y + 11, bg);
        fr.poly({ {x + 4, y + 5}, {x + 8, y + 2}, {x + 8, y + 8} }, ar);
        fr.poly({ {x + 26, y + 5}, {x + 22, y + 2}, {x + 22, y + 8} }, ar);
        fr.hline(x + 8, x + 22, y + 5, ar);
        text(fr, "L", x + 11, y + 3, lt); text(fr, "R", x + 17, y + 3, lt);
    }
}

void BedroomRound::drawEffects(Frame&, const Tetris&, int t) {
    int pf = f(t), ee = e(t);
    float flash = 0;                                                      // lightning
    if (pf == L0) flash = 1.0f; else if (pf == L0 + 1) flash = 0.35f; else if (pf == L0 + 3) flash = 0.85f; else if (pf == L0 + 4) flash = 0.25f;
    float moon = pf >= G0 ? ease((pf - G0) / 12.0f) : 0.0f;
    float tint = 0.12f * moon * (dry(t) ? 1 - ease((ee - GOO_DRY) / 12.0f) : 1.0f);
    float night = pf >= L0 ? 0.18f : 0.0f;                                // the lamp is off: darker
    if (endT_ >= 0 && ee >= DOOR_IN && ee < ALL_GONE + 5) night *= 0.4f;  // hallway light
    float black = 0;
    if (pf < 8) black = 1 - pf / 8.0f;
    if (endT_ >= 0 && ee >= END_FADE) black = std::min(1.0f, (ee - END_FADE + 1) / 10.0f);
    for (auto& c : palette()) {
        RGB v = mix(c, RGB{ 20, 22, 50 }, night);
        v = mix(v, RGB{ 210, 222, 255 }, 0.55f * flash);
        v = mix(v, RGB{ 40, 50, 110 }, tint);
        c = mix(v, RGB{ 0, 0, 0 }, black);
    }
}

// ---------------------------------------------------------------- logic
bool BedroomRound::endingDone() const { return endT_ >= 0 && lastT_ - endT_ >= END_LEN; }
bool BedroomRound::swapLeftRight() const {
    return eyeT_ >= 0 && lastT_ >= eyeT_ + 20 && (endT_ < 0 || lastT_ - endT_ < GOO_DRY);
}

void BedroomRound::reset() {
    started_ = false; lines_ = 0; clearT_ = -100; clearN_ = 1;
    eyeT_ = closT_ = chestT_ = endT_ = -1;
    stains_.clear(); nextStainT_ = snatchT_ = nextSnatchT_ = -1;
}

void BedroomRound::specialEvent(int t) { if (chestT_ >= 0 && snatchT_ < 0 && endT_ < 0) { snatchT_ = t; snatchSrc_ = (int)(rnd() % 2); } }

void BedroomRound::onLinesCleared(const std::vector<int>& rows, int totalLines, int t) {
    lines_ = std::min(20, totalLines);
    clearT_ = t; clearN_ = std::max(1, (int)rows.size());
    if (lines_ >= 3 && eyeT_ < 0) eyeT_ = t;                               // the eye flies in
    if (lines_ >= 7 && closT_ < 0) closT_ = t;                             // the werewolf
    if (lines_ >= 9 && chestT_ < 0) chestT_ = t;                           // the tentacles
    if (lines_ >= 20 && endT_ < 0) endT_ = t + 8;                          // the ending
}

void BedroomRound::control(Tetris& g, int t) {
    if (!started_) { started_ = true; start_ = t; }
    bool ending = endT_ >= 0;
    // stains: blocked cells for 7 s; a piece that lands on one is eaten and the stain grows by it
    stains_.erase(std::remove_if(stains_.begin(), stains_.end(), [&](const Stain& s) { return t - s.born >= 70 || ending; }), stains_.end());
    auto eaten = g.takeEaten();
    if (!eaten.empty()) {
        Stain* under = nullptr;
        for (auto& s : stains_) for (auto& q : s.cells) for (auto& c : eaten)
            if (q.first == c.first && q.second == c.second + 1) under = &s;
        for (auto& c : eaten)
            if (under && c.second >= 5) under->cells.push_back(c);        // never up into the spawn rows
    }
    if (closT_ >= 0 && !ending && playing(t)) {
        if (nextStainT_ < 0) nextStainT_ = closT_ + 14;
        if (t >= nextStainT_) {                                            // a new stain a few rows above the stack
            int c0 = 1 + (int)(rnd() % 7), top = 20;
            for (int y = 0; y < 20 && top == 20; ++y) if (g.at(c0, y) >= 0 || g.at(c0 + 1, y) >= 0) top = y;
            int r0 = std::max(6, top - 4);
            std::vector<std::pair<int, int>> cells;
            for (int dy = 0; dy < 2; ++dy) for (int dx = 0; dx < 2; ++dx)
                if (r0 + dy < 20 && g.at(c0 + dx, r0 + dy) < 0) cells.push_back({ c0 + dx, r0 + dy });
            for (auto& q : Tetris::cells(g.current(), g.rotation()))
                cells.erase(std::remove(cells.begin(), cells.end(), std::make_pair(g.px() + q.first, g.py() + q.second)), cells.end());
            if (!cells.empty()) stains_.push_back({ cells, t });
            nextStainT_ = t + 110 + (int)(rnd() % 60);
        }
    }
    std::vector<std::pair<int, int>> blocked;
    for (auto& s : stains_) for (auto& c : s.cells) blocked.push_back(c);
    g.setBlocked(blocked);
    // tentacles snatch the falling piece, more often near 20 lines
    if (chestT_ >= 0 && !ending && playing(t)) {
        if (nextSnatchT_ < 0) nextSnatchT_ = chestT_ + 20;
        if (snatchT_ < 0 && t >= nextSnatchT_ && !g.clearing()) {
            snatchT_ = t; snatchSrc_ = (int)(rnd() % 2);
            nextSnatchT_ = t + std::max(24, 70 - lines_ * 2) + (int)(rnd() % 30);
        }
    }
    if (snatchT_ >= 0) {
        int k = t - snatchT_;
        if (k == 6 && !ending) g.dropCurrent();                            // got it: the piece is gone
        if (k >= 14) snatchT_ = -1;
    }
    pieceX_ = BX + g.px() * 8; pieceY_ = BY + g.py() * 8;
}

void BedroomRound::update(const Tetris&, int t) {
    if (!started_) { started_ = true; start_ = t; }
    lastT_ = t;
}