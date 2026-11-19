#include "ui/HouseScene.h"
#include "ui/PixelFont.h"
#include <cmath>
namespace {
struct Box { int x, y, w, h; };
const std::vector<Box>& solids(HouseRooms::Room r) {
    static const std::vector<Box> living = { { 0, 0, 240, 66 }, { 54, 62, 28, 40 }, { 158, 62, 28, 40 }, { 102, 82, 36, 18 }, { 94, 110, 54, 24 }, { 208, 96, 22, 38 }, { 0, 0, 2, 96 }, { 0, 120, 2, 15 }, { 28, 62, 44, 6 } };
    static const std::vector<Box> kitchen = { { 0, 0, 240, 68 }, { 116, 0, 112, 70 }, { 208, 0, 20, 88 }, { 22, 56, 70, 72 }, { 0, 74, 20, 24 }, { 216, 118, 24, 17 }, { 238, 0, 2, 96 }, { 238, 120, 2, 15 } };
    static const std::vector<Box> library = { { 0, 0, 240, 58 }, { 0, 0, 16, 135 }, { 224, 0, 16, 135 }, { 52, 62, 38, 32 }, { 36, 76, 18, 14 }, { 90, 64, 18, 14 }, { 62, 96, 18, 14 }, { 158, 80, 34, 26 }, { 165, 66, 18, 14 }, { 165, 110, 18, 14 }, { 20, 64, 12, 22 }, { 208, 60, 12, 22 }, { 16, 102, 50, 26 }, { 174, 102, 50, 26 }, { 94, 78, 20, 30 }, { 16, 128, 88, 7 }, { 136, 128, 88, 7 } };
    return r == HouseRooms::LIVING ? living : r == HouseRooms::KITCHEN ? kitchen : library;
}
const RGB OL{ 42, 26, 28 };
}
void HouseScene::start(int stage, int t) {
    stage_ = stage; candyStep_ = 0; albumStep_ = 0; animT_ = -100;
    if (stage_ > 0) {
        phase_ = HUNGRY; phaseT_ = t; room_ = HouseRooms::LIBRARY; kx_ = 104; ky_ = 118; face_ = People::FRONT; momGone_ = true; doorOpenT_ = -1; libraryT_ = -1; note_.clear(); noteSub_.clear(); roomT_ = t;
        say({ { "", "...My stomach is growling." }, { "", "I should eat something first." } }, t);
        return;
    }
    phase_ = TALK; phaseT_ = t; room_ = HouseRooms::LIVING; kx_ = 40; ky_ = 100; face_ = People::BACK; momX_ = 17; momY_ = 70; libraryT_ = -1; momGone_ = false; doorOpenT_ = -1; note_.clear(); noteSub_.clear();
    say({ { "", "We're heading out for a bit." }, { "", "We'll be back soon, okay?" }, { "", "Oh, and I prepared something for you. It's in the kitchen." } }, t);
}
void HouseScene::say(const std::vector<Line>& lines, int t) { lines_ = lines; lineT_ = t; }
void HouseScene::advance(int t) {
    if (!talking()) return;
    int shown = (t - lineT_) * 2;
    if (shown < (int)lines_.front().text.size()) { lineT_ = t - 1000; return; }
    lines_.erase(lines_.begin()); lineT_ = t;
    if (talking()) return;
    if (phase_ == TALK) setPhase(MOM_LEAVE, t);
    else if (phase_ == HUNGRY) { setPhase(FREE, t); note_ = stage_ > 0 ? "Eat something first" : "Something for me..."; noteSub_ = "Go to the kitchen"; }
    else if (phase_ == CANDY && candyStep_ == 0) { candyStep_ = 1; animT_ = t; }
    else if (phase_ == CANDY && candyStep_ == 2) { if (stage_ > 0) { setPhase(ALBUM_HUNT, t); phaseT_ = t - 100; note_ = "Find the album"; noteSub_ = "Go to the library"; } else setPhase(WOBBLE, t); }
    else if (phase_ == WOBBLE) { setPhase(ALBUM_HUNT, t); note_ = "I forgot something..."; noteSub_ = "Go to the library"; }
    else if (phase_ == ALBUM && albumStep_ == 1) { albumStep_ = 2; animT_ = t; }
}
bool HouseScene::blocked(float x, float y) const {
    for (int dx : { -4, 4 }) {
        int px = (int)x + dx, py = (int)y;
        for (const Box& b : solids(room_)) if (px >= b.x && px < b.x + b.w && py >= b.y && py < b.y + b.h) return true;
    }
    return false;
}
void HouseScene::enter(HouseRooms::Room r, float x, float y, People::Facing f, int t) { room_ = r; kx_ = x; ky_ = y; face_ = f; roomT_ = t; }
void HouseScene::move(float dx, float dy, int t) {
    if (dx < 0) face_ = People::LEFT; else if (dx > 0) face_ = People::RIGHT; else if (dy < 0) face_ = People::BACK; else face_ = People::FRONT;
    float nx = kx_ + dx, ny = ky_ + dy;
    if (room_ == HouseRooms::LIVING && nx < 3 && ny >= 96 && ny < 120) { enter(HouseRooms::KITCHEN, 230, ny, People::LEFT, t); return; }
    if (room_ == HouseRooms::KITCHEN && nx > 237 && ny >= 96 && ny < 120) { enter(HouseRooms::LIVING, 10, ny, People::RIGHT, t); return; }
    if (room_ == HouseRooms::LIVING && ny < 70 && nx >= 216 && nx <= 234) { enter(HouseRooms::LIBRARY, 120, 124, People::BACK, t); return; }
    if (room_ == HouseRooms::LIBRARY && ny > 130 && nx >= 106 && nx <= 134) { enter(HouseRooms::LIVING, 225, 76, People::FRONT, t); return; }
    if (!blocked(nx, ny) && nx > 1 && nx < 239 && ny < 134) { kx_ = nx; ky_ = ny; }
    if (walkT_ != t) { ++step_; walkT_ = t; }
}
bool HouseScene::nearFood() const { return stage_ > 0 ? (room_ == HouseRooms::KITCHEN && kx_ >= 90 && kx_ <= 108 && ky_ >= 76 && ky_ <= 112) : nearCandy(); }
bool HouseScene::nearCandy() const { return room_ == HouseRooms::KITCHEN && kx_ >= 166 && kx_ <= 206 && ky_ <= 82; }
bool HouseScene::nearAlbum() const { return room_ == HouseRooms::LIBRARY && kx_ >= 88 && kx_ <= 122 && ky_ >= 106 && ky_ <= 122; }
void HouseScene::handle(Command k, int t) {
    if (!active()) return;
    if (k == Command::Confirm || k == Command::HardDrop) {
        if (talking()) { advance(t); return; }
        if (phase_ == FREE && nearFood()) {
            setPhase(CANDY, t); candyStep_ = 0;
            if (stage_ > 0) { face_ = People::LEFT; kx_ = 98; ky_ = 92; say({ { "", "A sandwich..." }, { "", "Someone left it for me." } }, t); }
            else { face_ = People::BACK; kx_ = 186; say({ { "", "Oh... a jar of candy." }, { "", "So this is what she prepared for me." } }, t); }
            return;
        }
        if ((phase_ == FREE) && nearAlbum()) { if (stage_ > 0) say({ { "", "I should eat something first." } }, t); else say({ { "", "Let me see what she prepared first." } }, t); return; }
        if (phase_ == ALBUM_HUNT && nearAlbum()) { setPhase(ALBUM, t); albumStep_ = 0; animT_ = t; kx_ = 104; ky_ = 112; face_ = People::BACK; return; }
        return;
    }
    if (talking() || (phase_ != FREE && phase_ != ALBUM_HUNT)) return;
    const float sp = 3;
    if (k == Command::MoveLeft) move(-sp, 0, t);
    else if (k == Command::MoveRight) move(sp, 0, t);
    else if (k == Command::Rotate) move(0, -sp, t);
    else if (k == Command::SoftDrop) move(0, sp, t);
}
void HouseScene::update(int t) {
    if (phase_ == MOM_LEAVE) {
        if (doorOpenT_ < 0) doorOpenT_ = t;
        if (t - doorOpenT_ > 4) momGone_ = true;
        if (t - doorOpenT_ > 10) { doorOpenT_ = -1; setPhase(HUNGRY, t); face_ = People::FRONT; say({ { "", "Something for me...?" }, { "", "Let's go and see." } }, t); }
    }
    if (phase_ == ALBUM_HUNT && room_ == HouseRooms::LIBRARY && libraryT_ < 0 && !talking()) { libraryT_ = t; if (stage_ > 0) say({ { "", "There it is." } }, t); else say({ { "", "...An album? On that chair." } }, t); }
    if (phase_ == WOBBLE && !talking() && t - phaseT_ == 8) say({ { "", "...Hm?" }, { "", "I feel like I forgot something in the library." } }, t);
    if (phase_ == CANDY && candyStep_ == 1 && t - animT_ >= 34) { candyStep_ = 2; if (stage_ > 0) say({ { "", "...It tastes weird." }, { "", "Oh well. The album is in the library." } }, t); else say({ { "", "...Sweet." } }, t); }
    if (phase_ == ALBUM && albumStep_ == 0 && t - animT_ >= 18) { albumStep_ = 1; if (stage_ > 0) say({ { "", "Let's keep going." } }, t); else say({ { "", "I don't think I've seen this album before." }, { "", "Let me take a look." } }, t); }
    if (phase_ == ALBUM && albumStep_ == 2 && t - animT_ >= 10) setPhase(FADE, t);
    if (phase_ == FADE && t - phaseT_ > 14) phase_ = DONE;
}
void HouseScene::drawHero(Frame& world, int x, int y, People::Facing face, int step, bool talk, bool blink) const {
    if (stage_ == 0) { People::drawKid(world, x, y, face, step, talk, blink); return; }
    static Frame tmp;
    uint8_t key = C(255, 0, 255);
    tmp.rect(0, 0, 40, 70, key);
    People::drawKid(tmp, 20, 60, face, step, talk, blink);
    uint8_t shirt = C(250, 210, 64), shirtD = C(208, 160, 44), shorts = C(72, 100, 170), shortsD = C(52, 74, 130);
    uint8_t skin = C(246, 204, 164), skinD = C(222, 168, 132), skinQ = C(204, 152, 120);
    uint8_t hood = C(90, 140, 210), hoodD = C(60, 100, 170), pants = C(46, 52, 84), pantsD = C(32, 36, 62);
    const int SH = 6, LEG = 9, LEGS = 15;
    for (int dy = -(32 + SH); dy <= 0; ++dy) {
        int sy, dyRel = dy;
        if (dyRel > -LEGS) { int k = dyRel + LEGS - 1; sy = 60 - LEG + 1 + k * LEG / LEGS; }
        else sy = 60 + dyRel + SH;
        bool leg = dyRel > -LEGS;
        for (int dx = -12; dx <= 12; ++dx) {
            uint8_t c = tmp.get(20 + dx, sy);
            if (c == key) continue;
            if (c == shirt) c = hood; else if (c == shirtD) c = hoodD;
            else if (c == shorts) c = pants; else if (c == shortsD) c = pantsD;
            else if (leg && (c == skin || c == skinD || c == skinQ)) c = c == skin ? pants : pantsD;
            world.set(x + dx, y + dy, c);
        }
    }
}
void HouseScene::draw(Frame& out, int t) const {
    static Frame world;
    bool candyTaken = phase_ > CANDY || (phase_ == CANDY && candyStep_ >= 1 && t - animT_ >= 5);
    bool albumOnChair = !(phase_ == FADE || (phase_ == ALBUM && (albumStep_ > 0 || t - animT_ >= 6)));
    HouseRooms::draw(world, room_, t, albumOnChair, candyTaken, stage_);
    auto sparkle = [&](int cx, int cy, int r, int seed) {
        for (int k = 0; k < 4; ++k) {
            int ph = (t + k * 3 + seed) % 12;
            if (ph > 5) continue;
            float an = k * 1.57f + seed;
            int sx = cx + (int)std::lround(std::cos(an) * r), sy = cy + (int)std::lround(std::sin(an) * r * 0.7f);
            int s = ph < 3 ? ph : 5 - ph;
            uint8_t c = C(255, 250, 200);
            world.set(sx, sy, C(255, 255, 255));
            for (int d = 1; d <= s; ++d) { world.set(sx + d, sy, c); world.set(sx - d, sy, c); world.set(sx, sy + d, c); world.set(sx, sy - d, c); }
        }
    };
    if (room_ == HouseRooms::KITCHEN && !candyTaken) { if (stage_ > 0) sparkle(57, 88, 12, 0); else sparkle(186, 32, 10, 0); }
    if (room_ == HouseRooms::LIBRARY && albumOnChair && phase_ >= ALBUM_HUNT) sparkle(104, 94, 12, 5);
    if (room_ == HouseRooms::LIVING && doorOpenT_ >= 0) { world.rect(7, 23, 20, 38, C(20, 14, 18)); world.rect(7, 23, 5, 38, C(150, 96, 60)); }
    bool momHere = room_ == HouseRooms::LIVING && !momGone_ && (phase_ == TALK || phase_ == MOM_LEAVE);
    bool momTalk = phase_ == TALK && talking();
    auto drawMom = [&]() { People::drawMom(world, (int)momX_, (int)momY_, phase_ == TALK ? People::FRONT : People::BACK, -1, momTalk && (t / 2) % 2); };
    bool walking = (phase_ == FREE || phase_ == ALBUM_HUNT) && t - walkT_ <= 1;
    bool kidBehind = momHere && momY_ > ky_;
    if (momHere && !kidBehind) drawMom();
    int kx = (int)kx_, ky = (int)ky_, an = t - animT_, sh = stage_ > 0 ? 6 : 0;
    int fx = stage_ > 0 ? 57 : 186, fy = stage_ > 0 ? 88 : 32;
    uint8_t skin = C(246, 204, 164), sleeve = C(250, 210, 64), olc = C(OL);
    auto arm = [&](int sx, int sy, int hx, int hy) { world.line(sx, sy, hx, hy, olc); world.line(sx + 1, sy, hx + 1, hy, olc); world.line(sx, sy + 1, (sx + hx) / 2, (sy + hy) / 2 + 1, sleeve); world.disc(hx, hy, 2, olc); world.disc(hx, hy, 1, skin); };
    auto foodAt = [&](int x, int y) { if (stage_ > 0) { world.rect(x - 4, y - 2, 9, 5, olc); world.rect(x - 3, y - 1, 7, 1, C(236, 196, 130)); world.rect(x - 3, y, 7, 1, C(110, 180, 80)); world.rect(x - 3, y + 1, 7, 1, C(236, 196, 130)); return; } world.rect(x - 1, y - 1, 3, 3, olc); world.set(x, y, C(240, 90, 120)); world.set(x - 2, y - 1, C(250, 210, 70)); world.set(x - 2, y + 1, C(250, 210, 70)); world.set(x + 2, y - 1, C(250, 210, 70)); world.set(x + 2, y + 1, C(250, 210, 70)); };
    auto bookAt = [&](int x, int y, bool open) {
        if (!open) { world.rect(x - 7, y - 5, 15, 11, olc); world.rect(x - 6, y - 4, 13, 9, C(150, 58, 70)); world.rect(x - 6, y - 4, 2, 9, C(118, 40, 54)); world.rect(x, y - 2, 4, 3, C(222, 184, 92)); }
        else { world.rect(x - 11, y - 5, 23, 10, olc); world.rect(x - 10, y - 4, 10, 8, C(250, 246, 232)); world.rect(x + 1, y - 4, 10, 8, C(250, 246, 232)); world.vline(x, y - 4, y + 3, C(150, 58, 70)); for (int k = 0; k < 3; ++k) { world.rect(x - 9, y - 3 + k * 2, 8, 1, C(214, 206, 190)); world.rect(x + 2, y - 3 + k * 2, 8, 1, C(214, 206, 190)); } }
    };
    if (phase_ == CANDY && candyStep_ == 1) {
        if (an < 14) {
            bool side = stage_ > 0;
            drawHero(world, kx, ky, side ? People::LEFT : People::BACK, -1, false, false);
            int hx = side ? kx - 6 - std::min(an, 5) * 2 : kx + 6, hy = side ? ky - 16 - sh : ky - 17 - sh - std::min(an, 5) * 3;
            arm(side ? kx - 2 : kx + 5, ky - 15 - sh, hx, hy);
            if (an >= 5) { float u = std::min(1.0f, (an - 5) / 8.0f); foodAt(fx + (int)std::lround((hx - fx) * u), fy + (int)std::lround((hy - 3 - fy) * u) - (int)std::lround(std::sin(u * 3.14f) * 8)); }
        } else {
            bool chew = an >= 21;
            drawHero(world, kx, ky, People::FRONT, -1, chew && (an / 2) % 2, false);
            if (an < 21) { int cy = an < 17 ? ky - 10 - sh : ky - 10 - sh - (an - 17) * 3; arm(kx - 4, ky - 15 - sh, kx - 1, cy); foodAt(kx - 1, cy - 2); }
            if (chew && (an / 3) % 2) { world.set(kx - 8, ky - 30 - sh, C(255, 250, 200)); world.set(kx + 8, ky - 31 - sh, C(255, 250, 200)); }
        }
    } else if (phase_ == CANDY && candyStep_ == 2) drawHero(world, kx, ky, People::FRONT, -1, false, true);
    else if (phase_ == ALBUM && albumStep_ == 0) {
        if (an < 12) {
            drawHero(world, kx, ky, People::BACK, -1, false, false);
            if (an < 6) arm(kx + 5, ky - 15 - sh, kx + 3, ky - 15 - sh - an * 2);
            else { float u = (an - 6) / 6.0f; int bx = 104, by = 92 + (int)std::lround((ky - 30 - sh - 92) * u); arm(kx - 4, ky - 15 - sh, kx - 3, by + 2); arm(kx + 5, ky - 15 - sh, kx + 4, by + 2); bookAt(bx, by, false); }
        } else { drawHero(world, kx, ky, People::FRONT, -1, false, false); bookAt(kx, ky - 13 - sh, false); arm(kx - 5, ky - 15 - sh, kx - 5, ky - 12 - sh); arm(kx + 5, ky - 15 - sh, kx + 5, ky - 12 - sh); }
    } else if (phase_ == ALBUM || phase_ == FADE) {
        bool open = phase_ == FADE || (albumStep_ == 2 && an >= 4);
        drawHero(world, kx, ky, People::FRONT, -1, talking() && (t / 2) % 2, false);
        if (albumStep_ == 2 && an < 4) bookAt(kx, ky - 13 - sh - an, false); else bookAt(kx, ky - 13 - sh, open);
        arm(kx - 5, ky - 15 - sh, kx - (open ? 9 : 5), ky - 12 - sh); arm(kx + 5, ky - 15 - sh, kx + (open ? 9 : 5), ky - 12 - sh);
        if (open) for (int k = 0; k < 3; ++k) if ((t + k * 2) % 6 < 3) world.set(kx - 8 + k * 8, ky - 22 - sh - (t + k) % 4, C(255, 250, 200));
    }
    else drawHero(world, kx, ky, face_, walking ? step_ : -1, talking() && phase_ != TALK && (t / 2) % 2, (t / 7) % 9 == 0);
    if (momHere && kidBehind) drawMom();
    bool prompt = !talking() && ((phase_ == FREE && nearCandy()) || ((phase_ == FREE || phase_ == ALBUM_HUNT) && nearAlbum()));
    if (prompt) { int bx = (int)kx_, by = (int)ky_ - 42 - (stage_ > 0 ? 6 : 0) - (t / 3) % 2; world.rect(bx - 3, by - 1, 7, 10, C(OL)); world.rect(bx - 2, by, 5, 8, C(250, 250, 240)); world.rect(bx, by + 1, 1, 4, C(OL)); world.set(bx, by + 6, C(OL)); }
    HouseRooms::zoomOut(out, world);
    if (phase_ == WOBBLE || (phase_ == ALBUM_HUNT && t - phaseT_ < 30)) {
        int a = phase_ == WOBBLE ? std::min(t - phaseT_, 20) : std::max(0, 30 - (t - phaseT_));
        float amp = a * 0.35f;
        static Frame copy; copy.px = out.px;
        for (int y = 0; y < Frame::H; ++y) {
            int sh = (int)std::lround(std::sin(y * 0.07f + t * 0.9f) * amp);
            for (int x = 0; x < Frame::W; ++x) out.set(x, y, copy.get(std::min(Frame::W - 1, std::max(0, x + sh)), y));
        }
        for (auto& c : palette()) c = mix(c, RGB{ 170, 120, 220 }, std::min(0.35f, amp * 0.04f));
    }
    if (!note_.empty() && !talking()) {
        int nx = 8, ny = 8, w = std::max(Font::width(note_), Font::width(noteSub_)) + 16;
        out.rect(nx + 2, ny + 2, w, 26, C(30, 20, 24)); out.rect(nx, ny, w, 26, C(OL)); out.rect(nx + 1, ny + 1, w - 2, 24, C(252, 244, 200));
        for (int k = 0; k < 3; ++k) out.hline(nx + 3, nx + w - 4, ny + 8 + k * 8, C(214, 222, 236));
        out.rect(nx + w / 2 - 8, ny - 3, 16, 6, C(220, 210, 170));
        Font::draw(out, note_, nx + 8, ny + 5, C(60, 70, 140));
        Font::draw(out, noteSub_, nx + 8, ny + 15, C(180, 70, 80));
    }
    if (talking()) {
        const Line& l = lines_.front();
        int shown = std::min((int)l.text.size(), std::max(0, (t - lineT_) * 2));
        int bx = 30, by = 196, bw = 420, bh = 64;
        out.rect(bx, by, bw, bh, C(250, 250, 250)); out.rect(bx + 3, by + 3, bw - 6, bh - 6, C(10, 8, 12));
        int tx = bx + 16;
        if (!l.who.empty()) { Font::draw(out, l.who, tx, by + 12, C(250, 220, 120)); tx += Font::width(l.who) + 10; }
        std::string s = l.text.substr(0, shown);
        int maxw = bx + bw - 16 - tx, line = 0;
        std::string cur;
        size_t i = 0;
        while (i <= s.size()) {
            size_t sp = s.find(' ', i); if (sp == std::string::npos) sp = s.size();
            std::string word = s.substr(i, sp - i);
            std::string tryS = cur.empty() ? word : cur + " " + word;
            if (Font::width(tryS) > maxw && !cur.empty()) { Font::draw(out, cur, tx, by + 12 + line * 12, C(250, 250, 250)); ++line; cur = word; }
            else cur = tryS;
            i = sp + 1;
        }
        Font::draw(out, cur, tx, by + 12 + line * 12, C(250, 250, 250));
        if (shown >= (int)l.text.size() && (t / 3) % 2) out.poly({ {bx + bw - 22, by + bh - 16}, {bx + bw - 14, by + bh - 16}, {bx + bw - 18, by + bh - 11} }, C(250, 250, 250));
    }
    if (t - roomT_ < 4) { float k = 1 - (t - roomT_) / 4.0f; for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k); }
    if (phase_ == FADE) { float k = std::min(1.0f, (t - phaseT_) / 12.0f); for (auto& c : palette()) c = mix(c, RGB{ 0, 0, 0 }, k); }
}
