#pragma once
#include "ui/RoundScene.h"
#include <deque>
using namespace std;

class WeddingRound : public RoundScene {
public:
    const char* name() const override { return "Wedding"; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return endT_ >= 0 && last_ - endT_ >= END; }
    bool playing(int) const override { return endT_ < 0; }
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int CHEER = 10, GUST = 30, RINGS = 16, KISS = 34, END = 84;
    deque<int> queue_;
    int cur_ = 0, curT_ = -1, last_ = 0, endT_ = -1, lines_ = 0, shown_ = 0, gustT_ = -100;
    void start(int t);
    void drawGarden(Frame& f, int t);
    void drawArch(Frame& f, int t);
    void drawGuests(Frame& f, int t, bool front);
};
