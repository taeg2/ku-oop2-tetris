#pragma once
#include "ui/RoundScene.h"
using namespace std;

class KitchenRound : public RoundScene {
public:
    const char* name() const override { return "Dinner"; }
    bool pillPanel() const override { return true; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return endStart_ >= 0 && endFrame_ >= FADE0 + 12; }
    void specialEvent(int t);
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame&, int, int) override {}
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int KID_LOOK = 4, KID_SAY0 = 6, KID_SAY1 = 24, MOM_TURN = 22, MOM_SAY0 = 26, MOM_SAY1 = 48;
    static const int MOM_WALK0 = 48, MOM_WALK1 = 107, DOOR_OPEN = 105, MOM_IN0 = 109, MOM_GONE = 117;
    static const int KID_STAND = 45, KID_HOP = 4, KID_WALK0 = 55, KID_WALK1 = 119, KID_IN0 = 121, KID_GONE = 129;
    static const int DOOR_CLOSE = 130, WORM0 = 2, WORM_END = 44, THROW = 12, THROW_LEN = 12;
    static const int BEAR0 = 136, BEAR_STAND = 174, BEAR_WALK0 = 177, BEAR_WALK1 = 274, KNOB = 278, BEAR_TURN = 288, FADE0 = 302;
    class Flyer { public: int start; int col[3]; float sx[3], sy[3], tx; };
    class Fish { public: int born; float sx; int col; };
    vector<Flyer> flyers_;
    vector<Fish> fish_;
    vector<int> bites_;
    vector<int> splashes_; vector<float> splashX_;
    int lastT_ = 0, earnedT_ = -1, endStart_ = -1, endFrame_ = 0, kickT_ = -100, lastNext_ = -1, endPiece_ = 0;
    int c() const { return endStart_ >= 0 ? endFrame_ : -1000; }
    void bitePose(int t, float& bite, bool& chew, int& eaten) const;
    void kidPos(int c, float& x, float& y) const;
    void bearPos(int c, float& x, float& y, int& seg) const;
    void drawFish(Frame& f, float x, float y, int col, int facing, int t);
    void drawTankLife(Frame& f, int t);
    void drawSoupPot(Frame& f, int t, bool stirring);
    void drawRoom(Frame& f);
    void drawLeftWindow(Frame& f);
    void drawFrontWindow(Frame& f);
    void drawDoor(Frame& f);
    void drawKitchen(Frame& f, int t);
    void drawTank(Frame& f, int t);
    void drawTable(Frame& f);
    void drawChair(Frame& f, int slide);
    void drawBowl(Frame& f, int x, int y, int eaten);
    void drawRug(Frame& f, int c, int t);
    void drawBear(Frame& f, int x, int y, int facing, int step, int reach, bool blink);
    void drawWorm(Frame& f, int x, int y, int t, bool facingLeft, int segCount = 7);
};
