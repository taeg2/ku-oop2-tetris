#pragma once
#include "ui/RoundScene.h"
#include <deque>
class OfficeRound : public RoundScene {
public:
    const char* name() const override { return "First Job"; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return endT_ >= 0 && last_ - endT_ >= END; }
    bool playing(int) const override { return endT_ < 0; }
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int DONE_LEN = 10, PILE_LEN = 28, STRETCH = 8, PAY = 20, LAMP = 56, END = 74;
    std::deque<int> queue_;
    int cur_ = 0, curT_ = -1, last_ = 0, endT_ = -1, lines_ = 0, shown_ = 0, inbox_ = 3, done_ = 0, pileT_ = -100;
    void start(int t);
    void drawWindow(Frame& f, int t);
    void drawCubicles(Frame& f, int t);
    void drawDesk(Frame& f, int t);
    void drawPapers(Frame& f, int x, int y, int n, bool messy);
};
