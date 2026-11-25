#include "ui/FadingRound.h"
#include "ui/LivingRoom.h"
#include "ui/AlbumScreen.h"
#include "ui/PixelFont.h"
#include "ui/SessionPlayState.h"
#include "ui/Stage2UI.h"
#include "ui/Stage3Art.h"
#include <algorithm>
#include <cmath>
using namespace Stage3Art;
static const int FLOOR_Y = 222, SOFA_X = 340, DOOR_X = 470;
static int rnd(float v) { return static_cast<int>(std::lround(v)); }
struct Expense { const char* what; int amount; };
static const Expense COSTS[10] = { {"RENT", -1200}, {"GROCERIES", -300}, {"PAYDAY", 2000}, {"SCHOOL FEES", -800}, {"CAR REPAIR", -600}, {"PAYDAY", 2000}, {"LOAN", -900}, {"TAXES", -700}, {"PAYDAY", 2000}, {"HOSPITAL", -500} };
static const RGB HAIRS[4][2] = { { {168, 92, 44}, {206, 128, 64} }, { {130, 84, 56}, {170, 120, 90} }, { {150, 136, 126}, {190, 180, 172} }, { {186, 182, 182}, {220, 218, 218} } };
static const char* SCENES[15] = { "THE LAKE", "WHERE ARE MY KEYS?", "OUR WEDDING", "DID I TURN OFF THE STOVE?", "HER DRAWING", "WHICH WAY IS HOME?", "GRADUATION", "WHAT WAS HER NAME?", "THE GOAL", "THE DOCTOR SAID... DEMENTIA", "FISH SOUP", "WHO ARE YOU?", "MY FIRST JOB", "...", "...?" };
static const int ICONS[15] = { 0, -1, 9, -1, 10, -1, 6, -1, 5, -1, 1, -1, 8, -1, -1 };
static void familyPhoto(Frame& f, int x, int y, int) {
    uint8_t ol = C(OLC), frame = C(200, 150, 90), sky = C(190, 220, 246);
    f.rect(x - 11, y - 9, 23, 19, ol); f.rect(x - 10, y - 8, 21, 17, frame); f.rect(x - 8, y - 6, 17, 13, sky); f.rect(x - 8, y + 4, 17, 3, C(130, 190, 100));
    auto who = [&](int px, int h, RGB hair, RGB top) { f.disc(px, y + 4 - h, 2, C(246, 204, 164)); f.set(px, y + 2 - h, C(hair)); f.set(px - 1, y + 2 - h, C(hair)); f.rect(px - 1, y + 6 - h, 3, h - 2, C(top)); };
    who(x - 5, 8, { 168, 92, 44 }, { 120, 150, 110 }); who(x, 8, { 120, 70, 46 }, { 214, 120, 140 }); who(x + 5, 5, { 120, 70, 46 }, { 250, 200, 90 });
}
void FadingRound::reset() { queue_.clear(); cur_ = 0; curT_ = -1; last_ = 0; endT_ = -1; lines_ = 0; shown_ = 0; laterT_ = -1; sweepT_ = -100; fastT_ = -100; money_ = 5000; evT_ = -100; ev_ = NONE; act_ = WANDER; actT_ = 0; px_ = 330; target_ = 384; wanderTo_ = 372; faceL_ = false; for (int& k : shred_) k = 0; }
void FadingRound::onLinesCleared(const std::vector<int>&, int totalLines, int) { while (lines_ < totalLines && lines_ < 25) queue_.push_back(++lines_); }
FadingRound::Event FadingRound::eventFor(int line) {
    static const Event BUSY[11] = { NONE, FASTER, BILLS, YEARS, DEADLINE, BILLS, YEARS, OVERTIME, BILLS, YEARS, FASTER };
    static const Event OLD[16] = { NONE, NONE, NONE, NONE, FORGOT, NONE, NONE, NONE, BLUR, NONE, NONE, NONE, STATIC, NONE, NONE, NONE };
    if (line <= PART1) return BUSY[line];
    return line - PART1 < 16 ? OLD[line - PART1] : NONE;
}
void FadingRound::start(int t) {
    cur_ = queue_.front(); queue_.pop_front(); curT_ = t; shown_ = cur_;
    if (cur_ <= PART1) money_ += COSTS[cur_ - 1].amount;
    ev_ = eventFor(cur_); evT_ = t + 4;
    for (int& k : shred_) k = 0;
    if (cur_ <= PART1) { act_ = ALERT; actT_ = t; target_ = ev_ == DEADLINE ? 340 : (ev_ == FASTER || ev_ == OVERTIME) ? 353 : ev_ == YEARS ? 306 : 384; }
    if (ev_ == BILLS) sweepT_ = evT_;
    if (ev_ == FASTER || ev_ == OVERTIME) fastT_ = evT_;
    if (cur_ == PART1) laterT_ = t + POP;
    if (cur_ == 25) { endT_ = t; ev_ = NONE; }
}
void FadingRound::stepBusy(int t) {
    int age = t - actT_;
    if (act_ == ALERT && age >= 5) { act_ = RUN; actT_ = t; }
    else if (act_ == RUN) { int d = target_ - px_; if (std::abs(d) <= 6) { px_ = target_; act_ = WORK; actT_ = t; faceL_ = target_ == 340 || target_ == 353 || target_ == 306; } else { px_ += d > 0 ? 6 : -6; faceL_ = d < 0; } }
    else if (act_ == WORK && age >= POP + 6) { act_ = WANDER; actT_ = t; }
    else if (act_ == PHONE && age >= 26) { act_ = WANDER; actT_ = t; }
    else if (act_ == WANDER) {
        if (age >= 70 && t % 97 == 0) { act_ = PHONE; actT_ = t; return; }
        if (t % 2 == 0) { int d = wanderTo_ - px_; if (d == 0) wanderTo_ = wanderTo_ == 372 ? 296 : 372; else { px_ += d > 0 ? 1 : -1; faceL_ = d < 0; } }
    }
    if ((ev_ == FASTER || ev_ == OVERTIME) && act_ == WORK && target_ == 353) { int c = (t * 7 + t / 3) % 10; shred_[c] = std::min(4, shred_[c] + 1); }
}
void FadingRound::update(const PlayStateReader&, int t) {
    last_ = t;
    if (shown_ <= PART1) stepBusy(t);
    if (endT_ >= 0) return;
    if (curT_ >= 0 && t - curT_ < 10) return;
    if (laterT_ >= 0 && t - laterT_ < LATER) return;
    if (!queue_.empty()) start(t);
}
float FadingRound::drain(int t) const {
    if (part2() < 3) return 0;
    float k = std::min(0.9f, (part2() - 2) / 10.0f * 0.9f);
    if (endT_ >= 0) { int e = t - endT_; k *= std::max(0.0f, 1.0f - std::max(0, e - DRINK) / 10.0f); }
    return k;
}
float FadingRound::forgotten(int t) const {
    if (part2() < 4) return 0;
    float now = std::min(1.0f, 0.15f + (part2() - 4) / 9.0f), before = part2() == 4 ? 0.0f : std::min(1.0f, 0.15f + (part2() - 5) / 9.0f);
    float k = curT_ >= 0 ? std::min(1.0f, (t - curT_) / 24.0f) : 1.0f;
    return before + (now - before) * k;
}
int FadingRound::fade(int t) const {
    (void)t;
    return 0;
}
static void wallBase(Frame& f, RGB w, RGB wd) {
    uint8_t ol = C(OLC), wall = C(w), wallD = C(wd), wain = C(150, 104, 70), wainL = C(178, 128, 90), floor = C(160, 112, 72), floorD = C(136, 92, 58);
    f.rect(0, 0, 480, FLOOR_Y, wall);
    for (int x = 0; x < 480; x += 24) f.vline(x, 0, FLOOR_Y - 30, wallD);
    f.rect(0, FLOOR_Y - 30, 480, 30, wain); f.hline(0, 479, FLOOR_Y - 31, ol); f.hline(0, 479, FLOOR_Y - 30, wainL);
    f.rect(0, FLOOR_Y, 480, 270 - FLOOR_Y, floor); f.hline(0, 479, FLOOR_Y, ol); for (int y = FLOOR_Y + 9; y < 270; y += 10) f.hline(0, 479, y, floorD);
    for (int y = FLOOR_Y + 1; y < 270; y += 10) for (int x = (y / 10 % 2) * 40; x < 480; x += 80) f.vline(x, y, std::min(269, y + 8), floorD);
}
static void wallClock(Frame& f, int cx, int cy, int spd, int t) {
    uint8_t ol = C(OLC);
    f.disc(cx, cy, 13, ol); f.disc(cx, cy, 12, C(250, 248, 238)); f.disc(cx, cy, 10, C(244, 240, 228));
    for (int k = 0; k < 4; ++k) f.set(cx + rnd(std::cos(k * 1.5708f) * 10), cy + rnd(std::sin(k * 1.5708f) * 10), ol);
    float am = t * 0.1f * spd, ah = am / 12;
    f.line(cx, cy, cx + rnd(std::cos(ah) * 5), cy + rnd(std::sin(ah) * 5), ol); f.line(cx, cy, cx + rnd(std::cos(am) * 9), cy + rnd(std::sin(am) * 9), C(200, 60, 60));
}
void FadingRound::drawOffice(Frame& f, int t) {
    wallBase(f, { 222, 214, 196 }, { 204, 196, 178 });
    uint8_t ol = C(OLC), wood = C(170, 120, 80), woodL = C(200, 150, 100), woodD = C(130, 88, 58);
    int hour = shown_ < 4 ? 0 : shown_ < 7 ? 1 : 2;
    wallClock(f, 222, 34, on(DEADLINE, t) ? 20 : 8, t);
    static const char* MONTHS[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
    int cxl = 204, cyl = 56, mon = (shown_ * 4) % 12;
    f.rect(cxl - 1, cyl - 1, 40, 42, ol); f.rect(cxl, cyl, 38, 40, C(250, 248, 240)); f.rect(cxl, cyl, 38, 11, C(210, 70, 70));
    Font::center(f, MONTHS[mon], cxl + 19, cyl + 3, C(250, 250, 250));
    for (int j = 0; j < 4; ++j) for (int i = 0; i < 5; ++i) f.rect(cxl + 3 + i * 7, cyl + 15 + j * 6, 4, 3, (i + j * 5) < (t / 3) % 20 ? C(210, 120, 120) : C(200, 196, 190));
    f.disc(cxl + 10, cyl - 1, 1, ol); f.disc(cxl + 28, cyl - 1, 1, ol);
    if (on(YEARS, t)) for (int k = 0; k < 4; ++k) { int a = (t - evT_) * 3 + k * 7, px = cxl + 19 + rnd(std::sin(a * 0.3f + k) * 14), py = cyl + 20 + a; if (py > FLOOR_Y) continue; f.rect(px - 6, py - 4, 13, 9, ol); f.rect(px - 5, py - 3, 11, 7, C(250, 248, 240)); f.rect(px - 5, py - 3, 11, 2, C(210, 70, 70)); }
    int bx0 = 258, bx1 = 300, by0 = 100;
    f.rect(bx0 - 1, by0 - 1, bx1 - bx0 + 2, FLOOR_Y - by0 + 1, ol); f.rect(bx0, by0, bx1 - bx0, FLOOR_Y - by0, woodD);
    static const RGB BOOK[6] = { {190, 70, 70}, {70, 110, 170}, {90, 150, 90}, {220, 180, 80}, {150, 100, 160}, {230, 230, 220} };
    for (int sy = by0 + 4; sy + 20 < FLOOR_Y; sy += 30) {
        int x = bx0 + 3;
        for (int k = 0; x < bx1 - 4; ++k) { int w = 3 + (k * 7 + sy) % 3, h = 14 + (k * 5 + sy) % 7; f.rect(x, sy + 22 - h, w, h, ol); f.rect(x, sy + 23 - h, w - 1, h - 1, C(BOOK[(k + sy / 30) % 6])); x += w + 1; }
        f.rect(bx0, sy + 22, bx1 - bx0, 3, wood); f.hline(bx0, bx1 - 1, sy + 22, woodL); f.hline(bx0, bx1 - 1, sy + 25, ol);
    }
    int px0 = 312, px1 = 372, py0 = 40, py1 = 92;
    f.rect(px0 - 2, py0 - 2, px1 - px0 + 4, py1 - py0 + 4, ol); f.rect(px0 - 1, py0 - 1, px1 - px0 + 2, py1 - py0 + 2, wood); f.rect(px0 + 1, py0 + 1, px1 - px0 - 2, py1 - py0 - 2, C(196, 156, 110));
    static const RGB NOTE[4] = { {252, 236, 120}, {250, 180, 190}, {170, 220, 250}, {180, 236, 160} };
    int notes = std::min(12, 3 + shown_);
    for (int k = 0; k < notes; ++k) { int nx = px0 + 4 + (k % 4) * 14, ny = py0 + 4 + (k / 4) * 15, lift = k == notes - 1 && curT_ >= 0 && t - curT_ < 6 ? 1 : 0; f.rect(nx, ny - lift, 11, 11, C(NOTE[k % 4])); f.hline(nx + 2, nx + 8, ny + 4 - lift, C(120, 110, 100)); f.hline(nx + 2, nx + 6, ny + 7 - lift, C(120, 110, 100)); f.set(nx + 5, ny - lift, C(220, 60, 60)); }
    int wx = 386, wy = 50, ww = 68, wh = 58;
    RGB sa = hour == 0 ? RGB{ 120, 180, 236 } : hour == 1 ? RGB{ 236, 150, 110 } : RGB{ 20, 26, 60 };
    RGB sb = hour == 0 ? RGB{ 206, 230, 250 } : hour == 1 ? RGB{ 250, 206, 150 } : RGB{ 50, 56, 100 };
    f.rect(wx - 4, wy - 4, ww + 8, wh + 8, ol); f.rect(wx - 3, wy - 3, ww + 6, wh + 6, C(246, 242, 232)); f.gradient(wx, wy, ww, wh, { sa, sb });
    if (hour == 0) { f.disc(wx + 50, wy + 14, 7, C(255, 244, 190)); f.ellipse(wx + 6 + (t / 2) % 56, wy + 30, 10, 3, C(250, 252, 255)); }
    if (hour == 1) f.disc(wx + 46, wy + wh - 10, 10, C(255, 214, 140));
    if (hour == 2) { f.disc(wx + 50, wy + 14, 6, C(250, 246, 210)); f.disc(wx + 53, wy + 12, 5, C(20, 26, 60)); for (int k = 0; k < 7; ++k) if ((t / 5 + k) % 4) f.set(wx + 4 + (k * 23) % 60, wy + 6 + (k * 17) % 40, C(250, 250, 230)); for (int k = 0; k < 5; ++k) { int bx = wx + 2 + k * 14, bh = 14 + (k * 11) % 18; f.rect(bx, wy + wh - bh, 12, bh, C(36, 40, 70)); for (int j = wy + wh - bh + 3; j < wy + wh - 2; j += 5) if ((j + k + t / 20) % 3) f.set(bx + 3 + (j % 2) * 5, j, C(250, 220, 120)); } }
    f.vline(wx + ww / 2, wy, wy + wh - 1, C(246, 242, 232)); f.hline(wx, wx + ww - 1, wy + wh / 2, C(246, 242, 232));
    int cab = 318;
    f.rect(cab - 16, FLOOR_Y - 32, 33, 32, ol); f.rect(cab - 15, FLOOR_Y - 31, 31, 31, C(150, 150, 160)); f.hline(cab - 15, cab + 15, FLOOR_Y - 16, ol); f.rect(cab - 3, FLOOR_Y - 26, 7, 2, ol); f.rect(cab - 3, FLOOR_Y - 10, 7, 2, ol);
    f.rect(cab - 14, FLOOR_Y - 46, 29, 14, ol); f.rect(cab - 13, FLOOR_Y - 45, 27, 12, C(220, 220, 226)); f.rect(cab - 11, FLOOR_Y - 41, 23, 2, ol); f.set(cab + 9, FLOOR_Y - 37, (t / 3) % 2 ? C(90, 220, 90) : C(40, 120, 40));
    int pp = (t * (curT_ >= 0 && t - curT_ < POP ? 3 : 1)) % 24;
    if (pp < 14) { f.rect(cab - 9, FLOOR_Y - 41 - pp / 2, 15, pp / 2 + 1, C(252, 252, 248)); f.hline(cab - 7, cab + 3, FLOOR_Y - 40 - pp / 2, C(150, 150, 160)); }
    int dx0 = 372, dx1 = 456, dy = FLOOR_Y - 36;
    f.rect(dx0, dy, dx1 - dx0, 5, ol); f.rect(dx0 + 1, dy + 1, dx1 - dx0 - 2, 3, wood); f.hline(dx0 + 1, dx1 - 2, dy + 1, woodL);
    f.rect(dx0 + 4, dy + 5, 4, FLOOR_Y - dy - 5, ol); f.rect(dx1 - 8, dy + 5, 4, FLOOR_Y - dy - 5, ol); f.rect(dx1 - 30, dy + 5, 22, 18, ol); f.rect(dx1 - 29, dy + 6, 20, 16, woodD); f.rect(dx1 - 21, dy + 12, 5, 2, woodL);
    f.rect(440, dy - 3, 12, 3, ol); f.rect(445, dy - 18, 3, 15, ol); f.rect(436, dy - 40, 5, 28, ol); f.rect(437, dy - 39, 3, 26, C(60, 64, 80)); f.vline(436, dy - 38, dy - 14, C(140, 200, 250)); f.vline(435, dy - 36, dy - 16, (t / 2) % 5 ? C(180, 220, 250) : C(120, 180, 230));
    f.rect(396, dy - 2, 20, 2, ol); f.line(398, dy - 3, 392, dy - 16, ol); f.line(399, dy - 3, 393, dy - 16, C(80, 84, 100)); f.line(400, dy - 3, 394, dy - 15, C(150, 210, 250));
    int pile = std::min(8, 2 + shown_ / 2);
    for (int k = 0; k < pile; ++k) { f.rect(418, dy - 2 - k * 2, 14, 2, ol); f.hline(419, 430, dy - 2 - k * 2, k % 3 == 1 ? C(250, 220, 220) : C(250, 250, 244)); }
    f.rect(378, dy - 8, 7, 8, ol); f.rect(379, dy - 7, 5, 6, C(240, 240, 236)); f.set(385, dy - 5, ol); f.set(386, dy - 4, ol); f.set(385, dy - 3, ol);
    for (int k = 0; k < 2; ++k) { int sy = dy - 12 - (t + k * 5) % 10; f.set(380 + k * 2 + ((sy / 3) % 2), sy, C(250, 250, 250)); }
    if (hour == 2) { f.line(452, dy, 446, dy - 22, ol); f.line(446, dy - 22, 432, dy - 18, ol); f.rect(428, dy - 21, 7, 4, C(60, 64, 80)); for (int y = dy - 16; y < dy; ++y) for (int x = 422 - (y - dy + 16) / 2; x < 438 + (y - dy + 16) / 2; ++x) if ((x + y) % 2 == 0 && f.get(x, y) != ol) f.set(x, y, C(250, 236, 170)); }
    int shx = 343;
    bool spit = (ev_ == FASTER || ev_ == OVERTIME) && act_ == WORK && target_ == 353;
    f.rect(shx - 7, FLOOR_Y - 20, 15, 20, ol); f.rect(shx - 6, FLOOR_Y - 19, 13, 19, C(70, 74, 90)); f.rect(shx - 7, FLOOR_Y - 24, 15, 5, ol); f.rect(shx - 6, FLOOR_Y - 23, 13, 3, C(150, 150, 160)); f.hline(shx - 4, shx + 4, FLOOR_Y - 22, ol); f.set(shx + 5, FLOOR_Y - 21, spit && (t % 2) ? C(250, 80, 80) : C(90, 200, 90));
    for (int k = 0; k < std::min(5, shown_ / 2 + (spit ? 3 : 0)); ++k) f.vline(shx - 4 + k * 2, FLOOR_Y - 18, FLOOR_Y - 12 + (k % 2) * 3, C(240, 240, 236));
    if (spit) for (int k = 0; k < 4; ++k) { int a = (t * 3 + k * 7) % 20; f.vline(shx - 3 + k * 2 + (a / 4) % 2, FLOOR_Y - 26 - a, FLOOR_Y - 24 - a, C(250, 250, 246)); }
    int cx = 368;
    f.rect(cx - 10, FLOOR_Y - 3, 21, 2, ol); f.disc(cx - 9, FLOOR_Y - 1, 1, ol); f.disc(cx + 9, FLOOR_Y - 1, 1, ol); f.rect(cx - 1, FLOOR_Y - 16, 3, 13, ol);
    f.rect(cx - 9, FLOOR_Y - 20, 19, 5, ol); f.rect(cx - 8, FLOOR_Y - 19, 17, 3, C(60, 64, 80));
    f.rect(cx - 12, FLOOR_Y - 44, 5, 25, ol); f.rect(cx - 11, FLOOR_Y - 43, 3, 23, C(60, 64, 80));
}
void FadingRound::drawLiving(Frame& f, int t) { LivingRoom::draw(f, t, SOFA_X, DOOR_X, on(STATIC, t), on(BLUR, t) ? -6 : 1); }
void FadingRound::drawBusy(Frame& f, int t) {
    uint8_t ol = C(OLC);
    int ag = std::min(3, shown_ / 3);
    Look me = HERO_HOME;
    me.top = { 236, 236, 230 }; me.topD = { 200, 200, 196 }; me.tie = true; me.accent = { 190, 50, 60 };
    me.hair = HAIRS[ag][0]; me.hairL = HAIRS[ag][1];
    if (ag >= 2) { me.top = { 110, 120, 100 }; me.topD = { 84, 92, 76 }; me.tie = false; }
    if (ag >= 3) me.glasses = true;
    bool desk = target_ == 384, printer = target_ == 340, shred = target_ == 353;
    Pose pose = act_ == WANDER || act_ == RUN ? WALK : STAND;
    Arm arm = act_ == PHONE ? UP : act_ == WORK ? (desk ? TYPE : printer ? HOLD : shred ? TYPE : REST) : act_ == RUN ? HOLD : HOLD;
    Mood mood = act_ == WANDER ? SMILE : act_ == PHONE ? TALK : act_ == ALERT ? SAD : act_ == RUN ? SAD : (target_ == 306 ? SAD : TALK);
    int hx = 0, hy = 0, tt = act_ == RUN ? t * 2 : act_ == WANDER ? t / 2 : t;
    person(f, px_, FLOOR_Y, me, faceL_, pose, arm, mood, tt, 2, &hx, &hy);
    int head = FLOOR_Y - 92;
    if (act_ == PHONE) { f.rect(hx - 2, hy - 2, 4, 8, ol); f.rect(hx - 1, hy - 1, 2, 6, C(60, 64, 80)); if (t - actT_ < 10 && (t / 2) % 2) Font::center(f, "RING RING", px_, head - 6, C(200, 80, 80), ol); }
    else if (arm == HOLD || (act_ == WORK && printer)) { for (int k = 0; k < 3; ++k) { f.rect(hx - 6, hy - 4 - k * 2, 13, 2, ol); f.hline(hx - 5, hx + 5, hy - 4 - k * 2, C(252, 252, 246)); } }
    if (act_ == WORK && shred && (t / 2) % 2) { f.rect(hx - 3, hy - 3, 7, 5, ol); f.rect(hx - 2, hy - 2, 5, 3, C(252, 252, 246)); }
    if (act_ == WANDER) { int nx = px_ + (faceL_ ? -12 : 12), ny = head + 6 - (t / 3) % 6; uint8_t nc = C(90, 70, 120); f.vline(nx + 2, ny - 5, ny, nc); f.rect(nx, ny, 3, 2, nc); f.hline(nx + 2, nx + 4, ny - 5, nc); f.set(nx + 4, ny - 4, nc); }
    if (act_ == ALERT) { Font::center(f, "!", px_, head - 2 - (t - actT_ < 2 ? 2 : 0), C(230, 60, 60), ol); }
    if (act_ == RUN) { int back = faceL_ ? 14 : -26; for (int k = 0; k < 3; ++k) f.hline(px_ + back + (t + k) % 3, px_ + back + 10, FLOOR_Y - 46 + k * 12, C(250, 250, 250)); for (int k = 0; k < 2; ++k) { int sy = head + 10 + (t + k * 3) % 6; f.set(px_ + (faceL_ ? 10 : -10) + k * 3, sy, C(150, 200, 250)); f.set(px_ + (faceL_ ? 10 : -10) + k * 3, sy + 1, C(150, 200, 250)); } }
    if (act_ == WORK && desk && (t / 3) % 2) Font::center(f, "PAYING...", px_ + 8, head - 6, C(80, 80, 90), ol);
    if (act_ == WORK && target_ == 306) Font::center(f, "WHERE DID THE YEARS GO?", 280, head - 6, C(250, 240, 210), ol);
}
void FadingRound::drawThought(Frame& f, int t) {
    if (curT_ < 0 || t - curT_ >= THINK || endT_ >= 0 || part2() < 1) return;
    uint8_t ol = C(OLC), cloud = C(252, 250, 244), ink = C(90, 70, 80);
    int a = t - curT_, m = part2(), cx = 320, cy = 66;
    for (int k = 0; k < 3; ++k) { int r = 2 + k; f.disc(SOFA_X - 4 - k * 8, FLOOR_Y - 90 - k * 8, r + 1, ol); f.disc(SOFA_X - 4 - k * 8, FLOOR_Y - 90 - k * 8, r, cloud); }
    f.ellipse(cx, cy, 84, 40, ol); f.ellipse(cx, cy, 83, 39, cloud);
    for (int k = 0; k < 12; ++k) { int px = cx + rnd(std::cos(k * 0.5236f) * 80), py = cy + rnd(std::sin(k * 0.5236f) * 36); f.disc(px, py, 8, ol); f.disc(px, py, 7, cloud); }
    f.ellipse(cx, cy, 80, 36, cloud);
    int icon = ICONS[(m - 1) % 15];
    if (icon >= 0) {
        static Frame tmp;
        uint8_t key = C(255, 0, 255);
        tmp.rect(0, 0, 60, 46, key);
        AlbumScreen::drawIcon(tmp, icon, 30, 22);
        int hole = m >= 8 ? std::min(36, a * 3) : 0;
        for (int y = 0; y < 38; ++y) for (int x = 4; x < 56; ++x) {
            uint8_t c = tmp.get(x, y + 2);
            if (c == key) continue;
            unsigned hsh = (static_cast<unsigned>(x) * 73856093u) ^ (static_cast<unsigned>(y) * 19349663u) ^ (static_cast<unsigned>(m) * 83492791u);
            if (static_cast<int>(hsh % 40) < hole) continue;
            f.set(cx - 26 + (x - 4), cy - 26 + y, c);
        }
        f.rect(cx - 27, cy - 27, 54, 1, ol); f.rect(cx - 27, cy + 12, 54, 1, ol); f.rect(cx - 27, cy - 27, 1, 40, ol); f.rect(cx + 26, cy - 27, 1, 40, ol);
        Font::center(f, m >= 8 && a > 14 ? "...?" : SCENES[(m - 1) % 15], cx, cy + 18, ink);
    } else {
        Font::center(f, "?", cx, cy - 18, C(160, 120, 120));
        Font::center(f, SCENES[(m - 1) % 15], cx, cy - 2, C(150, 60, 60));
    }
}
void FadingRound::drawOld(Frame& f, int t) {
    uint8_t ol = C(OLC);
    const RGB chairC{ 232, 200, 172 };
    armchairBack(f, SOFA_X, FLOOR_Y, chairC);
    int e = endT_ >= 0 ? t - endT_ : -1;
    Mood m = e >= DRINK ? SMILE : e >= 0 ? SAD : (curT_ >= 0 && t - curT_ < THINK && part2() < 6) ? SMILE : part2() >= 6 ? SAD : CALM;
    Arm arm = e >= OFFER && e < DRINK ? OUT : e >= DRINK && e < PHOTO ? HOLD : REST;
    int hx = 0, hy = 0;
    person(f, SOFA_X, FLOOR_Y, HERO_OLD, false, SIT, arm, m, t, 2, &hx, &hy);
    armchairFront(f, SOFA_X, FLOOR_Y, chairC);
    uint8_t glass = C(210, 232, 248), water = C(150, 200, 240);
    if (e >= DRINK && e < PHOTO) { f.rect(hx - 2, hy - 8, 6, 9, ol); f.rect(hx - 1, hy - 7, 4, 7, glass); f.rect(hx - 1, hy - 4, 4, 4, water); }
    if (e >= 0) {
        float u = std::min(1.0f, e / static_cast<float>(WALK_IN));
        int dx = rnd(DOOR_X + (SOFA_X + 44 - DOOR_X) * u);
        int dhx = 0, dhy = 0;
        bool hasTray = e < DRINK;
        person(f, dx, FLOOR_Y, DAUGHTER, true, e < WALK_IN ? WALK : STAND, hasTray ? (e >= OFFER ? OUT : HOLD) : REST, e >= DRINK ? SMILE : TALK, t, 2, &dhx, &dhy);
        if (hasTray) {
            int tx = dhx - 6, ty = dhy - 2;
            f.rect(tx - 10, ty, 20, 3, ol); f.rect(tx - 9, ty + 1, 18, 1, C(200, 200, 206));
            f.rect(tx - 7, ty - 9, 6, 9, ol); f.rect(tx - 6, ty - 8, 4, 7, glass); f.rect(tx - 6, ty - 5, 4, 4, water);
            f.rect(tx + 2, ty - 3, 6, 3, ol); f.rect(tx + 3, ty - 2, 2, 1, C(70, 130, 230)); f.rect(tx + 5, ty - 2, 2, 1, C(250, 250, 246));
        }
    }
}
void FadingRound::drawMiniBoard(Frame& f, const PlayStateReader& g, int t) {
    for (int y = 50; y < 230; ++y) for (int x = 90; x < 190; ++x) f.set(x, y, room_.get(x, y));
    uint8_t ol = C(42, 26, 28), wl = C(224, 180, 118), w = C(156, 104, 62), wd = C(114, 72, 50);
    int W = 10 * CS, H = 20 * CS;
    f.rect(MX - 6, MY - 6, W + 12, H + 12, ol); f.rect(MX - 5, MY - 5, W + 10, H + 10, wl); f.rect(MX - 3, MY - 3, W + 6, H + 6, w); f.rect(MX - 2, MY - 2, W + 4, H + 4, wd); f.rect(MX - 1, MY - 1, W + 2, H + 2, ol);
    for (int x = MX + 4; x < MX + W - 4; x += 9) f.hline(x, x + 3, MY - 3, wd);
    for (int y = MY + 6; y < MY + H - 6; y += 11) { f.vline(MX - 3, y, y + 4, wd); f.vline(MX + W + 2, y + 3, y + 7, wd); }
    int cx[2] = { MX - 6, MX + W }, cy[2] = { MY - 6, MY + H };
    for (int i = 0; i < 2; ++i) for (int j = 0; j < 2; ++j) { f.rect(cx[i], cy[j], 6, 6, ol); f.rect(cx[i] + 2, cy[j] + 2, 2, 2, C(194, 142, 88)); }
    f.rect(MX, MY, W, H, C(22, 26, 42));
    for (int x = CS; x < W; x += CS) f.vline(MX + x, MY, MY + H - 1, C(32, 38, 58));
    for (int y = CS; y < H; y += CS) f.hline(MX, MX + W - 1, MY + y, C(32, 38, 58));
    for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x) if (g.at(x, y) >= 0) ToyBlocks::cell6(f, MX + x * CS, MY + y * CS, g.over() ? 7 : g.at(x, y));
    if (g.clearing()) { uint8_t c = (t / 2) % 2 ? C(255, 236, 140) : C(250, 248, 255); for (int y : g.clearingRows()) f.rect(MX, MY + y * CS, W, CS, c); return; }
    if (g.over() || !g.hasPiece() || !playing(t)) return;
    for (auto& c : g.ghostCells()) if (c.second >= 0) { const RGB* r = ToyBlocks::ramp(g.current()); uint8_t c3 = C(r[3]); int gx = MX + c.first * CS, gy = MY + c.second * CS; for (int i = 0; i < CS; i += 2) { f.set(gx + i, gy, c3); f.set(gx, gy + i, c3); f.set(gx + i + 1, gy + CS - 1, c3); f.set(gx + CS - 1, gy + i + 1, c3); } }
    for (auto& c : g.pieceCells()) if (c.second >= 0) ToyBlocks::cell6(f, MX + c.first * CS, MY + c.second * CS, g.current());
}
void FadingRound::drawBackground(Frame& f, const PlayStateReader&, int t) {
    bool office = !old(t) || (laterT_ >= 0 && t - laterT_ >= 0 && t - laterT_ < LATER / 2);
    if (office) drawOffice(f, t); else drawLiving(f, t);
    Stage2UI::pills(f, t);
    drawCommonUI(f);
    if (!office) room_ = f;
}
void FadingRound::drawForeground(Frame& f, const PlayStateReader& g, int t) {
    if (laterT_ >= 0 && t - laterT_ >= 0 && t - laterT_ < LATER / 2) { drawBusy(f, t); return; }
    if (!old(t)) { drawBusy(f, t); return; }
    drawMiniBoard(f, g, t);
    drawOld(f, t);
    drawThought(f, t);
}
void FadingRound::drawNext(Frame& f, int piece, int t) {
    uint8_t ol = C(OLC), note = C(252, 236, 140);
    bool z = old(last_);
    int x = z ? 111 : 198, y = z ? 206 : 132;
    f.rect(x - 1, y - 1, 52, 46, ol); f.rect(x, y, 50, 44, note); f.hline(x, x + 49, y + 43, C(226, 206, 110)); f.disc(x + 25, y, 2, C(220, 60, 60));
    bool blank = part2() >= 11 && endT_ < 0;
    Font::center(f, blank && (t / 6) % 2 ? "N?XT" : "NEXT", x + 25, y + 4, C(110, 90, 60));
    if (blank) { Font::center(f, "?", x + 25, y + 22, C(160, 140, 110)); return; }
    std::vector<std::pair<int, int>> cs = SessionPlayState::shapeOf(piece);
    int mx = 9, my = 9, Mx = 0, My = 0;
    for (auto& c : cs) { mx = std::min(mx, c.first); my = std::min(my, c.second); Mx = std::max(Mx, c.first); My = std::max(My, c.second); }
    int bw = (Mx - mx + 1) * 6, bh = (My - my + 1) * 6;
    for (auto& c : cs) ToyBlocks::cell6(f, x + 25 - bw / 2 + (c.first - mx) * 6, y + 26 - bh / 2 + (c.second - my) * 6, piece);
}
void FadingRound::drawHud(Frame& f, int, int t) {
    uint8_t ol = C(OLC), paper = C(250, 246, 232), ink = C(80, 70, 80), life = C(110, 170, 90), mem = C(240, 180, 80), dim = C(206, 200, 190);
    bool second = old(t);
    if (!second) {
        int x = 392, y = 6;
        f.rect(x - 1, y - 1, 86, 34, ol); f.rect(x, y, 84, 32, C(40, 60, 50));
        Font::center(f, "WALLET", x + 42, y + 4, C(200, 230, 200));
        std::string amt = "$" + std::to_string(std::max(0, money_));
        Font::center(f, amt, x + 42, y + 17, money_ < 2000 ? C(250, 120, 110) : C(250, 236, 140));
    }
    int x = 4, y = 60, n = second ? 15 : 10, got = second ? std::max(0, part2()) : shown_;
    f.rect(x - 1, y - 1, 86, 56, ol); f.rect(x, y, 84, 54, paper);
    const char* label = !second ? "BUSY LIFE" : part2() >= 12 && endT_ < 0 ? ((t / 5) % 3 == 0 ? "WHO AM I" : (t / 5) % 3 == 1 ? "REMEM..." : "WHERE?") : "MEMORIES";
    Font::center(f, label, x + 42, y + 4, ink);
    for (int i = 0; i < n; ++i) { int cx = x + 10 + (i % 5) * 16, cy = y + 18 + (i / 5) * 11; f.disc(cx, cy, 4, ol); f.disc(cx, cy, 3, i < got ? (second ? mem : life) : dim); }
}
void FadingRound::drawEffects(Frame& f, const PlayStateReader& g, int t) {
    uint8_t ol = C(OLC);
    bool z = old(t);
    int bx = z ? MX : boardX(), by = z ? MY : boardY(), cs = z ? CS : 8, BW = cs * 10, BH = cs * 20, e = endT_ >= 0 ? t - endT_ : -1;
    if (!old(t) && curT_ >= 0 && t - curT_ >= 5 && t - curT_ < POP + 5 && shown_ >= 1 && shown_ <= PART1) {
        int a = t - curT_ - 5;
        const Expense& ex = COSTS[shown_ - 1];
        bool gain = ex.amount > 0;
        std::string txt = std::string(ex.what) + (gain ? " +$" : " -$") + std::to_string(std::abs(ex.amount));
        int w = Font::width(txt) + 12, px = 434 - w / 2, py = 44 + std::min(a, 6);
        f.rect(px - 1, py - 1, w + 2, 13, ol); f.rect(px, py, w, 11, C(252, 250, 244)); f.rect(px, py, 4, 11, gain ? C(80, 170, 90) : C(210, 70, 70));
        Font::draw(f, txt, px + 8, py + 3, gain ? C(60, 140, 70) : C(170, 50, 50));
        for (int k = 0; k < 5; ++k) { int mx = 434 + (k - 2) * 12 + a * (k % 2 ? 2 : -2), my = gain ? 120 - a * 3 + k * 4 : 70 + a * 5 + k * 6; if (my < 0 || my > 260) continue; f.rect(mx - 5, my - 3, 11, 6, ol); f.rect(mx - 4, my - 2, 9, 4, C(120, 190, 110)); f.set(mx, my, C(70, 130, 70)); }
    }
    if (!old(t) && shown_ <= PART1) {
        int top[10];
        for (int x = 0; x < 10; ++x) { top[x] = 20; for (int y = 0; y < 20; ++y) if (g.at(x, y) >= 0) { top[x] = y; break; } }
        uint8_t paper = C(250, 250, 244), line = C(170, 190, 220);
        bool falling = (ev_ == FASTER || ev_ == OVERTIME) && act_ == WORK && target_ == 353;
        if (falling) for (int k = 0; k < 8; ++k) { int col = (k * 7 + 3) % 10, x = bx + col * 8 + 2 + (k % 3), surf = by + (top[col] - std::min(2, shred_[col] / 2)) * 8 - 5, span = std::max(10, surf - by), y = by + (k * 41 + t * 4) % span, w = rnd(std::sin((y + k * 13) * 0.25f) * 2); if (y >= surf) continue; f.rect(x + w - 1, y - 1, 4, 7, ol); f.rect(x + w, y, 2, 5, paper); f.set(x + w, y + 2, line); f.set(x + w + 1, y + 4, line); }
        for (int c = 0; c < 10; ++c) {
            int full = std::min(2, shred_[c] / 2), bits = shred_[c] - full * 2;
            for (int i = 0; i < full; ++i) {
                int row = top[c] - 1 - i;
                if (row < 0) continue;
                bool hit = false;
                for (auto& pc : g.pieceCells()) if (pc.first == c && pc.second == row) hit = true;
                if (hit) continue;
                int x = bx + c * 8, y = by + row * 8;
                f.rect(x, y, 8, 8, ol); f.rect(x + 1, y + 1, 6, 6, paper); f.hline(x + 1, x + 4, y + 2, line); f.hline(x + 3, x + 6, y + 4, line); f.set(x + 2, y + 6, line); f.set(x + 6, y + 1, C(214, 214, 220)); f.set(x + 1, y + 6, C(214, 214, 220));
            }
            int sy = by + (top[c] - full) * 8 - 2;
            for (int i = 0; i < bits && sy > by; ++i) { int x = bx + c * 8 + 1 + i * 2; f.rect(x, sy, 2, 2, paper); f.set(x, sy + 1, line); }
        }
        if (t - fastT_ >= 0 && t - fastT_ < FAST && ((t - fastT_) / 3) % 2 == 0) Font::center(f, ev_ == OVERTIME ? "OVERTIME! SHRED IT ALL!" : "FASTER! SHRED IT!", bx + BW / 2, by - 18, C(250, 220, 120), ol);
    }
    if (t - sweepT_ >= 0 && t - sweepT_ < SWEEP && !old(t)) {
        int a = t - sweepT_, ry = by + 144, ex = bx - 18 + a * 6;
        for (int y = ry; y < ry + 16; ++y) for (int x = bx; x < std::min(bx + BW, ex); ++x) if ((x + y) % 2) f.set(x, y, C(200, 60, 60));
        for (int k = 0; k < 2; ++k) { int px = ex + k * 6, py = ry - 2 + k * 9; f.rect(px - 1, py - 1, 16, 11, ol); f.rect(px, py, 14, 9, C(250, 236, 236)); f.rect(px, py, 14, 3, C(210, 70, 70)); }
        if ((a / 3) % 2 == 0) Font::center(f, "BILLS! -2 ROWS", bx + BW / 2, by - 18, C(250, 120, 110), ol);
        if (a > 8) Font::center(f, "NO POINTS", bx + BW / 2, by + 132, C(250, 200, 190), ol);
    }
    if (curT_ >= 0 && t - curT_ < 12 && !old(t) && shown_ % 3 == 0 && shown_ < PART1) Font::center(f, "YEARS GO BY", 320, 104, C(250, 240, 210), ol);
    if (laterT_ >= 0 && t - laterT_ >= 0 && t - laterT_ < LATER) {
        int a = t - laterT_;
        float k = a < LATER / 2 ? a / (LATER / 2.0f) : (LATER - a) / (LATER / 2.0f);
        for (auto& c : palette()) c = mix(c, RGB{ 10, 8, 12 }, k * 0.92f);
        if (a > 14 && a < LATER - 8) Font::center(f, "MANY YEARS LATER...", 240, 120, C(250, 240, 220));
        return;
    }
    int fd = fade(t);
    if (fd > 0) {
        uint8_t bg = C(22, 26, 42);
        for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x) {
            if (g.at(x, y) < 0) continue;
            bool piece = false;
            for (auto& c : g.pieceCells()) if (c.first == x && c.second == y) piece = true;
            if (piece) continue;
            for (int j = 0; j < cs; ++j) for (int i = 0; i < cs; ++i) { int s = (i + j + t / 4) % 4; if (fd >= 2 ? s != 0 : s % 2 == 0) f.set(bx + x * cs + i, by + y * cs + j, bg); }
        }
    }
    if (on(DEADLINE, t)) {
        int a = t - evT_;
        uint8_t red = C(220, 50, 50);
        if ((a / 2) % 2 == 0) { for (int k = 0; k < 4; ++k) { f.hline(0, 479, k, red); f.hline(0, 479, 269 - k, red); f.vline(k, 0, 269, red); f.vline(479 - k, 0, 269, red); } }
        Font::center(f, "DEADLINE!", bx + BW / 2, by - 18, (a / 3) % 2 ? C(250, 120, 110) : C(250, 236, 140), ol);
        if (a > 6) Font::center(f, "DUE TODAY", bx + BW / 2, by + 70, C(250, 200, 190), ol);
    }
    float fg = forgotten(t);
    if (fg > 0) {
        uint8_t bg = C(22, 26, 42), gl = C(32, 38, 58), poof = C(250, 250, 255);
        for (int y = 0; y < PlayStateReader::kRows; ++y) for (int x = 0; x < PlayStateReader::kCols; ++x) {
            if (g.at(x, y) < 0) continue;
            bool piece = false;
            if (playing(t)) for (auto& pc : g.pieceCells()) if (pc.first == x && pc.second == y) piece = true;
            if (piece) continue;
            unsigned hsh = (static_cast<unsigned>(x) * 2654435761u) ^ (static_cast<unsigned>(PlayStateReader::kRows - y) * 40503u);
            float v = (hsh % 1000) / 1000.0f;
            if (v >= fg) continue;
            int cx = bx + x * cs, cy = by + y * cs;
            if (fg - v < 0.04f && endT_ < 0 && curT_ >= 0 && t - curT_ < 24 && fg < 0.999f) { f.rect(cx, cy, cs, cs, poof); continue; }
            f.rect(cx, cy, cs, cs, bg); f.hline(cx, cx + cs - 1, cy, gl); f.vline(cx, cy, cy + cs - 1, gl);
        }
        if (on(FORGOT, t) && ((t - evT_) / 3) % 2 == 0) Font::center(f, "FORGETTING...", bx + BW / 2, by - 18, C(220, 210, 240), ol);
    }
    if (on(BLUR, t)) {
        int a = t - evT_, r = a < 8 ? 1 + a / 3 : a > dur() - 8 ? 1 + (dur() - a) / 3 : 3;
        std::vector<uint8_t> copy(BW * BH);
        for (int y = 0; y < BH; ++y) for (int x = 0; x < BW; ++x) copy[y * BW + x] = f.get(bx + x, by + y);
        for (int y = 0; y < BH; ++y) for (int x = 0; x < BW; ++x) {
            unsigned h = (static_cast<unsigned>(x) * 73856093u) ^ (static_cast<unsigned>(y) * 19349663u) ^ (static_cast<unsigned>(t / 2) * 83492791u);
            int dx = static_cast<int>(h % (2 * r + 1)) - r, dy = static_cast<int>((h >> 8) % (2 * r + 1)) - r;
            int sx = std::min(BW - 1, std::max(0, x + dx)), sy = std::min(BH - 1, std::max(0, y + dy));
            f.set(bx + x, by + y, copy[sy * BW + sx]);
        }
        if ((a / 3) % 2 == 0) Font::center(f, "EVERYTHING'S BLURRY", bx + BW / 2, by - 18, C(230, 230, 230), ol);
    }
    if (on(STATIC, t)) {
        if (frozenT_ != evT_) { frozenT_ = evT_; frozen_.assign(BW * BH, RGB{ 0, 0, 0 }); for (int y = 0; y < BH; ++y) for (int x = 0; x < BW; ++x) frozen_[y * BW + x] = palette()[f.get(bx + x, by + y)]; }
        else for (int y = 0; y < BH; ++y) for (int x = 0; x < BW; ++x) f.set(bx + x, by + y, C(frozen_[y * BW + x]));
    }
    if (on(STATIC, t)) {
        int a = t - evT_;
        for (int y = by; y < by + BH; ++y) for (int x = bx; x < bx + BW; ++x) { unsigned h = (static_cast<unsigned>(x) * 2654435761u) ^ (static_cast<unsigned>(y) * 40503u) ^ (static_cast<unsigned>(t) * 97u); if (h % 9 == 0) f.set(x, y, C(240, 240, 240)); else if (h % 9 == 1) f.set(x, y, C(90, 90, 96)); }
        int band = by + (a * 13) % BH;
        for (int y = band; y < std::min(by + BH, band + 6); ++y) for (int x = bx; x < bx + BW; ++x) if ((x + t) % 3) f.set(x, y, C(200, 200, 206));
        Font::center(f, "SSSHHHH...", bx + BW / 2, by - 18, C(230, 230, 230), ol);
    }
    if (e >= 0 && e < BLACK) {
        if (e >= 4 && e < WALK_IN) Font::center(f, "DAD, TIME FOR YOUR MEDICINE.", 380, 100, C(250, 250, 240), ol);
        if (e >= WALK_IN && e < OFFER) Font::center(f, "...WHO ARE YOU?", SOFA_X, 100, C(230, 230, 240), ol);
        if (e >= OFFER && e < DRINK) Font::center(f, "IT'S ME, DAD. HERE, WITH WATER.", 380, 112, C(250, 236, 200), ol);
        if (e >= DRINK && e < DRINK + 12) for (int r = 0; r < 3; ++r) for (int a = 0; a < 16; ++a) { int rr = (e - DRINK) * 10 + r * 12; star(f, SOFA_X + rnd(std::cos(a * 0.39f) * rr), FLOOR_Y - 60 + rnd(std::sin(a * 0.39f) * rr * 0.7f)); }
        if (e >= PHOTO - 8 && e < PHOTO) familyPhoto(f, SOFA_X + 20, 140 - (e - PHOTO + 8), t);
        flyToBook(f, e - PHOTO, SOFA_X + 20, 132, familyPhoto, "FAMILY!", t);
    }
    float k = drain(t);
    if (k > 0) {
        bool keep[256] = {};
        int pad = z ? 6 : 7;
        for (int y = by - pad; y < by + BH + pad; ++y) for (int x = bx - pad; x < bx + BW + pad; ++x) keep[f.get(x, y)] = true;
        auto& pal = palette();
        for (std::size_t i = 0; i < pal.size() && i < 256; ++i) { if (keep[i]) continue; RGB v = pal[i]; int lum = static_cast<int>(v.r * 0.3f + v.g * 0.59f + v.b * 0.11f); pal[i] = mix(v, RGB{ lum, lum, lum + 6 }, k); }
    }
    if (e >= BLACK - 10) {
        float q = std::min(1.0f, (e - BLACK + 10) / 10.0f);
        if (e >= BLACK) { std::fill(f.px.begin(), f.px.end(), C(0, 0, 0)); q = 0; }
        for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, q);
        auto line = [&](int start, const char* text, int y) {
            if (e < start) return;
            std::string s = text;
            int n = std::min(static_cast<int>(s.size()), (e - start) * 2);
            Font::center(f, s.substr(0, n), 240, y + 12, C(250, 250, 250));
        };
        if (e >= BLACK) {
            line(TALK1, "...Thank you, sweetheart.", 80);
            line(TALK2, "You remember me?", 120);
            line(TALK3, "How could I ever forget you.", 160);
        }
        if (e >= END - 14) { float out = std::min(1.0f, (e - END + 15) / 14.0f); for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, out); }
    }
}
