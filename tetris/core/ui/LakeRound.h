#pragma once
#include "ui/RoundScene.h"
using namespace std;

class LakeRound : public RoundScene {
public:
    const char* name() const override { return "Breeze"; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    static const int FADE0 = 114;
    bool endingDone() const override { return endStart_ >= 0 && endFrame_ >= FADE0 + 12; }
    void specialEvent(int t);
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    class Flyer { public: float x0, y0; int start; int type; };
    vector<Flyer> flyers_;
    int arrived_ = 0;
    int memoryT_ = -1;
    int windStart_ = -1, gusts_ = 0;
    float boatUp_ = 0;
    int endStart_ = -1, endFrame_ = 0;
    float boatX_ = 308, boatY_ = 150, rowFromX_ = 308, rowFromY_ = 150, cloudX_ = 0;
    float wind(int t) const;
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
