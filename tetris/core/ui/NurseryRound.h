#pragma once
#include "ui/RoundScene.h"
#include <deque>
using namespace std;

class NurseryRound : public RoundScene {
public:
    const char* name() const override { return "Our Baby"; }
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
    static const int STEP = 10, CRY = 40, GIVE = 22, END = 76;
    deque<int> queue_;
    int cur_ = 0, curT_ = -1, last_ = 0, endT_ = -1, lines_ = 0, shown_ = 0, cryT_ = -100;
    void start(int t);
    int phase() const { return shown_ < 3 ? 0 : shown_ < 6 ? 1 : 2; }
    int dark() const { return phase() == 0 ? 3 : phase() == 1 ? 1 : 0; }
    void drawRoom(Frame& f, int t);
    void drawCrib(Frame& f, int t);
};
