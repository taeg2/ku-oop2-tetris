// BeachRound.h - Stage 1, Round 3.
#pragma once
#include "Scene.h"

// Round 3: sand castles at sunset on the lake beach. The board stands in a sand mound with a turret on top.
// NEXT = an octopus's tentacles holding the piece up out of a hole in the sand. HUD = his upside-down bucket:
// one starfish per line. The kid builds the castles himself: every cleared row = 4 pats = half a castle
// (2 rows = 1 castle, 10 lines = 5 castles). Memory: his shovel and bucket. Ending: "HONEY! TIME FOR BED!",
// he waves bye to the octopus and runs off; then the castles leak black goo and the tentacle turns black.
// The beach, castles, octopus and bucket are drawn with code; the kid is hand-typed sprites (People.h).
class BeachRound : public Scene {
public:
    const char* name() const override { return "Sand Castles"; }
    int boardX() const override { return 380; }
    int boardY() const override { return 62; }
    void reset() override;
    void update(const Tetris& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override { return goneT_ >= 0 && lastT_ >= goneT_ + 4 + 52 + 10; }
    void specialEvent(int t) override;   // W key: one more pending row for the kid (demo)
protected:
    void drawBackground(Frame& f, const Tetris& g, int t) override;
    void drawBoardFrame(Frame& f, int t) override;
    void drawForeground(Frame& f, const Tetris& g, int t) override;
    void drawNext(Frame&, int, int) override {}     // the tentacles are drawn with the sand items (sorted by depth)
    void drawHud(Frame&, int, int) override {}      // so is the bucket
    void drawEffects(Frame& f, const Tetris& g, int t) override;
private:
    enum Mode { Idle, Build, Happy, Stand, Run, Kneel, End };
    Mode mode_ = Idle;
    int mt_ = 0, cur_ = 0, pending_ = 0;            // mode timer, castle he's on, rows waiting to be built
    float kx_ = 0, ky_ = 0;                         // his knees / feet on the sand
    std::vector<std::pair<float, float>> path_;
    int halves_[5] = {}, part_[5] = {};             // halves done per castle, pats done in the current half
    int pose_ = 0;                                  // which kid sprite (index into a flat list, see .cpp)
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
