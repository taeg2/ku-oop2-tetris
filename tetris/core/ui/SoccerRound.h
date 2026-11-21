#pragma once
#include "ui/RoundScene.h"
#include <deque>
class SoccerRound : public RoundScene {
public:
    const char* name() const override { return "Soccer"; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return goalT_ >= 0 && last_ - goalT_ >= END; }
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    enum Kind { PASS, BLOCK, GOAL };
    struct Ev { Kind k; int line; };
    static const int PASS_LEN = 10, SHOT_LEN = 9, BOUNCE_LEN = 10, BLOCK_LEN = SHOT_LEN + BOUNCE_LEN, CHEER = 14, PAN_UP = 30, LAPSE = 52, PAN_DOWN = 140, HOLD = 164, END = 240;
    std::deque<Ev> queue_;
    Ev cur_{ PASS, 0 };
    int curT_ = -1, holder_ = 0, shooter_ = 0, passes_ = 0, saves_ = 0, last_ = 0, goalT_ = -1, dropT_ = -100, dropY_ = 0;
    bool drop_ = false, armed_ = false;
    void start(int t);
    void track(const PlayStateReader& g, int t);
    static int pieceTop(const PlayStateReader& g);
    void ballPos(int t, float& x, float& y, float& h) const;
    void drawSchool(Frame& f, int t);
    void drawField(Frame& f);
    void drawGoal(Frame& f, int t, bool front);
    void drawKeeper(Frame& f, int t);
    void drawPlayers(Frame& f, int t);
    void drawSky(Frame& f, int g);
    void drawPlaza(Frame& f, int t);
    void drawFirstDay(Frame& f, int t);
    bool older(int t) const { return goalT_ >= 0 && t - goalT_ >= PAN_DOWN; }
    int lines_ = 0;
};
