#pragma once
#include "ui/RoundScene.h"
#include <deque>
using namespace std;

// Stage 3 보스 라운드. 1부(10줄): 집 안 작업실에서 뛰어다니는 바쁜 어른. 돈이 나가고, 마감, 야근, 청구서가 아래 두 줄을 쓸어 간다
// 2부(15줄): 거실 소파에 앉은 노인. 안개, 잊힌 줄, TV 잡음, 흔들림이 오고, 마지막에 딸이 약과 물을 가져다주면 화면이 어두워지고 대화가 나온다
class FadingRound : public RoundScene {
public:
    const char* name() const override { return "Today"; }
    int goal() const override { return 25; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return endT_ >= 0 && last_ - endT_ >= END; }
    bool playing(int t) const override { return endT_ < 0 && !(laterT_ >= 0 && t - laterT_ < LATER); }
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    static const int PART1 = 10, POP = 20, SWEEP = 18, FAST = 20, LATER = 50, THINK = 30, EV = 26;
    enum Event { NONE, FASTER, BILLS, YEARS, DEADLINE, OVERTIME, FORGOT, BLUR, STATIC };
    static const int WALK_IN = 30, OFFER = 46, DRINK = 62, PHOTO = 70, BLACK = 96, TALK1 = 104, TALK2 = 130, TALK3 = 156, END = 196;
    deque<int> queue_;
    int cur_ = 0, curT_ = -1, last_ = 0, endT_ = -1, lines_ = 0, shown_ = 0, laterT_ = -1, sweepT_ = -100, fastT_ = -100, money_ = 5000, evT_ = -100;
    Event ev_ = NONE;
    enum Act { WANDER, ALERT, RUN, WORK, PHONE };
    Act act_ = WANDER;
    int actT_ = 0, px_ = 330, target_ = 384, wanderTo_ = 372, shred_[10] = {};
    bool faceL_ = false;
    void stepBusy(int t);
    void start(int t);
    int part2() const { return shown_ - PART1; }
    bool old(int t) const { return shown_ > PART1 || (laterT_ >= 0 && t - laterT_ >= LATER / 2); }
    float drain(int t) const;
    int fade(int t) const;
    static Event eventFor(int line);
    int dur() const { return ev_ == BLUR || ev_ == STATIC ? 44 : EV; }
    bool on(Event k, int t) const { return ev_ == k && t - evT_ >= 0 && t - evT_ < dur(); }
    float forgotten(int t) const;
    vector<RGB> frozen_;
    int frozenT_ = -1000;
    void drawOffice(Frame& f, int t);
    void drawLiving(Frame& f, int t);
    void drawMiniBoard(Frame& f, const PlayStateReader& g, int t);
    static const int MX = 106, MY = 76, CS = 6;
    Frame room_;
    void drawBusy(Frame& f, int t);
    void drawOld(Frame& f, int t);
    void drawThought(Frame& f, int t);
};
