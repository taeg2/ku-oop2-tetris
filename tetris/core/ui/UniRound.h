#pragma once
#include "ui/RoundScene.h"
#include <deque>
using namespace std;

class UniRound : public RoundScene {
public:
    const char* name() const override { return "Uni"; }
    int goal() const override { return 12; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return gradT_ >= 0 && last_ - gradT_ >= END; }
    bool playing(int t) const override;
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawBoardFrame(Frame& f, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    enum Kind { TYPE, GRAD };
    class Ev { public: Kind k; int line; };
    static const int TYPE_LEN = 8, DAWN = 64, STAND = 10, WHITE_OUT = 22, WHITE_IN = 34, CEREMONY = 44, RAISE = 58, TOSS = 72, END = 132;
    deque<Ev> queue_;
    Ev cur_{ TYPE, 0 };
    int curT_ = -1, last_ = 0, gradT_ = -1, lines_ = 0, shown_ = 0, season_ = 0, cups_ = 0, nightT_ = -1000, nightLine_ = 0, lineT_ = 0;
    void start(int t);
    int night(int t) const;
    bool ceremony(int t) const { return gradT_ >= 0 && t - gradT_ >= WHITE_IN; }
    void drawSky(Frame& f, int t, int n);
    void drawCampus(Frame& f, int t, int n);
    void drawTree(Frame& f, int t);
    void drawStudent(Frame& f, int t, int n);
    void drawWalkers(Frame& f, int t);
    void drawWeather(Frame& f, int t);
    void drawHall(Frame& f, int t);
    void drawCeremony(Frame& f, int t);
    void drawBench(Frame& f);
    void nightLamp(Frame& f, const PlayStateReader& g);
    void petalGust(Frame& f, int t);
    void sunGlare(Frame& f, int t);
    void frost(Frame& f, int t);
    bool nextBlown(int t) const;
};
