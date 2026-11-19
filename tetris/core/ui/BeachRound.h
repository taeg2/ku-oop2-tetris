#pragma once
#include "ui/RoundScene.h"
class BeachRound : public RoundScene {
public:
    const char* name() const override { return "Sand Castles"; }
    int boardX() const override { return 380; }
    int boardY() const override { return 62; }
    void reset() override;
    void update(const PlayStateReader& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return goneT_ >= 0 && lastT_ >= goneT_ + 4 + 52 + 10; }
    void specialEvent(int t);
protected:
    void drawBackground(Frame& f, const PlayStateReader& g, int t) override;
    void drawBoardFrame(Frame& f, int t) override;
    void drawForeground(Frame& f, const PlayStateReader& g, int t) override;
    void drawNext(Frame&, int, int) override {}
    void drawHud(Frame&, int, int) override {}
    void drawEffects(Frame& f, const PlayStateReader& g, int t) override;
private:
    enum Mode { Idle, Build, Happy, Stand, Run, Kneel, End };
    Mode mode_ = Idle;
    int mt_ = 0, cur_ = 0, pending_ = 0;
    float kx_ = 0, ky_ = 0;
    std::vector<std::pair<float, float>> path_;
    int halves_[5] = {}, part_[5] = {};
    int pose_ = 0;
    int puffT_ = -1, puffX_ = 0, puffY_ = 0;
    int startT_ = 0, lastT_ = 0, lines_ = 0, clearT_ = -100, clearN_ = 0;
    int endT_ = -1, goneT_ = -1;
    int piece_ = 0, prevPiece_ = 0;
    float build(int i) const { return (halves_[i] + part_[i] / 4.0f) / 2.0f; }
    void kneelPoint(int i, float& x, float& y) const;
    void patReach(int i, int& dx, int& h) const;
    float pathLen() const;
    void along(float s, float& x, float& y) const;
    void drawKid(Frame& f, int t);
    void drawSky(Frame& f, int t);
    void drawCastle(Frame& f, int i, float b, int t);
    void drawCastleGoo(Frame& f, int i, int k);
    void drawTentacle(Frame& f, int x0, int y0, int x1, int y1, int bend, float w0, int t, float black);
    void drawShovelBucket(Frame& f, int x, int y);
    void drawBucket(Frame& f, int t);
    void drawOcto(Frame& f, int t);
};
