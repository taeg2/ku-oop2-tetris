#pragma once
#include "ui/RoundScene.h"
#include <deque>
class InterviewRound : public RoundScene {
public:
    const char* name() const override { return "Interview"; }
    int goal() const override { return 20; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return passT_ >= 0 && last_ - passT_ >= END; }
    bool playing(int) const override { return passT_ < 0; }
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int ASK = 22, BABBLE = 16, Q_LEN = ASK + BABBLE, SMILE = 18, WELCOME = 34, END = 76;
    std::deque<int> queue_;
    int pieceNo_ = 0, lastTop_ = -1, cur_ = 0, curT_ = -1, last_ = 0, passT_ = -1, lines_ = 0, shown_ = 0, lineT_ = 0;
    bool odd_ = false, nextOdd_ = false;
    void start(int t);
    bool asking(int t) const { return passT_ < 0 && curT_ >= 0 && t - curT_ < ASK; }
    bool babbling(int t) const { return passT_ < 0 && curT_ >= 0 && t - curT_ >= ASK && t - curT_ < Q_LEN; }
    bool blank(int t) const;
    int whiteout(int t) const;
    void drawRoom(Frame& f, int t);
    void drawClock(Frame& f, int t);
    void drawExaminer(Frame& f, int t);
    void drawMain(Frame& f, int t);
    void drawTable(Frame& f);
    void drawPaperBlocks(Frame& f, const PlayStateReader& g, int t);
    void warpBoard(Frame& f, int t);
    static void bubble(Frame& f, int x, int y, int w, const char* text, bool tailRight, uint8_t ink);
};
