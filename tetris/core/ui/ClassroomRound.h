#pragma once
#include "ui/RoundScene.h"
class ClassroomRound : public RoundScene {
public:
    const char* name() const override { return "First Day"; }
    void reset() override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    void update(const PlayStateReader& g, int t) override;
    bool endingDone() const override { return endT_ >= 0 && last_ - endT_ >= SIGN0 + 12; }
protected:
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame& f, int piece, int t) override;
    void drawHud(Frame& f, int lines, int t) override;
private:
    static const int RING1 = 20, CLOSE = 12, STAND0 = 22, STAND1 = 28, REACH = 32, LIFT = 40, TO_BAG = 44, GRAB = 58, WEAR = 63, TO_DOOR = 95, DOOR_SHUT = 105, SIGN0 = 111;
    int endT_ = -1, last_ = 0;
    void drawDoor(Frame& f, int t);
    void drawBell(Frame& f, int t);
    void drawBagOnHook(Frame& f, int n, int t);
    void pose(int e, int& x, int& top, int& step, bool& back) const;
    void drawCarry(Frame& f, int t);
    int lines_ = 0, clearT_ = -100, shown_ = -1, prev_ = -1, swapT_ = -100, bagT_ = -100, piece_ = 0;
    bool bag_ = false, give_ = false, used_ = false, saw_ = false;
    int gifts_ = 0, gift_ = 0;
    void drawWindow(Frame& f, int t);
    void drawBlackboard(Frame& f);
    void drawCabinet(Frame& f);
    void drawSideDesk(Frame& f);
    void drawDesk(Frame& f);
    void drawStudent(Frame& f, int t);
    void drawSupplies(Frame& f);
};
