// LakeRound.h - Stage 1, Round 1.
#pragma once
#include "Scene.h"

// Round 1: a breezy day at the lake. NEXT = the angel teddy's towel, HUD = chalkboard.
// Each line: the row flashes, 4 blocks fly to the kid in the boat, +1 chalk tally. 10 lines = THE BOAT.
// Every 4 cleared lines a gust of wind blows the towel away (the teddy catches it). Ending: mom calls him in for dinner.
class LakeRound : public Scene {
public:
    const char* name() const override { return "Breeze"; }
    void reset() override;
    void update(const Tetris& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    static const int FADE0 = 114;
    bool endingDone() const override { return endStart_ >= 0 && endFrame_ >= FADE0 + 12; }
    bool drawsOwnClearScreen() const override { return false; }
    void specialEvent(int t) override;   // W key: a gust of wind right now
protected:
    void drawBackground(Frame& f, const Tetris& g, int t) override;
    void drawForeground(Frame& f, const Tetris& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const Tetris& g, int t) override;
private:
    struct Flyer { float x0, y0; int start; int type; };
    std::vector<Flyer> flyers_;
    int arrived_ = 0;            // tallies on the chalkboard (blocks that reached the kid)
    int memoryT_ = -1;           // when THE BOAT was earned
    int windStart_ = -1, gusts_ = 0;   // a gust every 4 cleared lines
    float boatUp_ = 0;           // the wind pushes the boat north
    int endStart_ = -1, endFrame_ = 0;
    float boatX_ = 308, boatY_ = 150, rowFromX_ = 308, rowFromY_ = 150, cloudX_ = 0;

    float wind(int t) const;     // 0..1 strength of the current gust
    int windFrame(int t) const { return windStart_ < 0 ? -1 : t - windStart_; }
    void kidAt(int& x, int& y) const;
    void drawTree(Frame& f, int t);
    void drawLake(Frame& f, int t);
    void drawCabin(Frame& f, int t);
    void drawStump(Frame& f);
    void drawBoat(Frame& f, int t);
    void drawWorm(Frame& f, int t);
    void drawWalkingKid(Frame& f, int feetX, int feetY, int facing, int step);
    void drawWindFx(Frame& f, int t);
    void drawSittingTeddy(Frame& f, int t, int k);
};
