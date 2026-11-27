#pragma once
#include "ui/RoundScene.h"
using namespace std;

class BedroomRound : public RoundScene {
public:
    const char* name() const override { return "Bedtime"; }
    bool pillPanel() const override { return true; }
    int goal() const override { return 20; }
    int boardX() const override { return 96; }
    int boardY() const override { return 80; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void track(const PlayStateReader& g, int t);
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override;
    bool playing(int t) const override { return started_ && t - start_ >= PLAY0 && (endT_ < 0 || t < endT_); }
    bool swapLeftRight() const;
    void specialEvent(int t);
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame&, int, int) override {}
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int MOM_TALK0 = 6, MOM_TALK1 = 30, MOM_LEAVE = 32, DOOR_SHUT = 38, L0 = 62, B0 = 72, BOOK_FLY0 = 80, BOOK_FLY1 = 96;
    static const int G0 = 98, GOO1_0 = 102, GOO1_1 = 120, JB = 124, FIGHT1 = 138, DROP1 = 145, WG0 = 151, GOO2_1 = 167, S0 = 174, PLAY0 = 182;
    static const int SHAKE2 = 16, SHAKE3 = 32, TENT_HIDE = 21, GOO_DRY = 37, DOOR_IN = 53;
    static const int TALK1 = 61, TALK2 = 83, TALK3 = 105, TALK_LEN = 20, KID_UP = 127, CRAWL1 = 143, RISE1 = 149, HOP1 = 157, LOOK1 = 163, KID_WALK0 = 165, KID_DOOR = 199;
    static const int ALL_GONE = 205, END_FADE = 216, END_LEN = 228;
    class Stain { public: vector<pair<int, int>> cells; int born; };
    bool started_ = false;
    int start_ = 0, lastT_ = 0, lines_ = 0, clearT_ = -100, clearN_ = 1;
    int eyeT_ = -1, closT_ = -1, chestT_ = -1, endT_ = -1;
    vector<Stain> stains_;
    int nextStainT_ = -1, snatchT_ = -1, nextSnatchT_ = -1, snatchSrc_ = 0;
    int pieceX_ = 0, pieceY_ = 0;
    unsigned rng_ = 12345;
    int f(int t) const { return t - start_; }
    int e(int t) const { return endT_ >= 0 ? t - endT_ : -1; }
    unsigned rnd() { rng_ = rng_ * 1103515245u + 12345u; return (rng_ >> 8) & 0xFFFFFF; }
    void drawRoom(Frame& fr);
    void drawWindow(Frame& fr, int pf, int ee);
    void drawShelf(Frame& fr, int pf);
    void drawDoor(Frame& fr, int t);
    void drawCloset(Frame& fr, int t);
    void drawChest(Frame& fr, int t);
    void drawHoodie(Frame& fr, int t);
    void drawBed(Frame& fr, int t);
    void drawToys(Frame& fr);
    void drawSoldiers(Frame& fr, int t);
    void drawCocoon(Frame& fr, int t);
    void drawBoardGoo(Frame& fr, int t);
    void drawStains(Frame& fr, int t);
    void drawIntro(Frame& fr, int pf);
    void drawEnding(Frame& fr, int t);
    void drawTentacles(Frame& fr, int t);
    void drawMist(Frame& fr, int t, float k);
    void drawTentacle(Frame& fr, float bx, float by, float tx, float ty, int t, float w0, float w1, float curl, int seed);
    void drawMoonPatch(Frame& fr, float k);
    float tentOut(int t) const;
    bool wolfOut(int t) const;
    bool dry(int t) const { return endT_ >= 0 && e(t) >= GOO_DRY; }
};
