// BedroomRound.h - Stage 1, Round 4: bedtime, the boss.
#pragma once
#include "Scene.h"

// Round 4: his bedroom at night. Intro: mom says goodnight, lightning, the book that would show NEXT flies off
// the shelf and the goo swallows it (no NEXT this round), goo crawls in through the window and covers the top of
// the board. 20 lines, one toy soldier per line.
//   3 lines: an eye flies in and lands in his hoodie -> LEFT and RIGHT are swapped.
//   7 lines: the werewolf leans out of the closet; its stains block cells, eat pieces that land on them, last 7 s.
//   9 lines: tentacles from the chest and from under the bed snatch the falling piece.
// Ending: the door shakes 3 times, the monsters hide, the parents come in and talk to him, he crawls out from under
// the quilt, hops off the bed and walks out with them, then the screen fades to black. Everything is drawn here
// with code (BedroomPaint.h), and the people and the toy soldiers are hand-typed sprites (People.h and BedroomRound.cpp).
class BedroomRound : public Scene {
public:
    const char* name() const override { return "Bedtime"; }
    int goal() const override { return 20; }
    int boardX() const override { return 96; }
    int boardY() const override { return 80; }
    void reset() override;
    void update(const Tetris& g, int t) override;
    void control(Tetris& g, int t) override;
    void onLinesCleared(const std::vector<int>& rows, int totalLines, int t) override;
    bool endingDone() const override;
    bool drawsOwnClearScreen() const override { return false; }
    bool playing(int t) const override { return started_ && t - start_ >= PLAY0 && (endT_ < 0 || t < endT_); }
    bool swapLeftRight() const override;
    void specialEvent(int t) override;   // W key: a snatch right now (when the tentacles are out)
protected:
    void drawBackground(Frame& f, const Tetris& g, int t) override;
    void drawForeground(Frame& f, const Tetris& g, int t) override;
    void drawNext(Frame&, int, int) override {}          // no NEXT this round: the goo swallowed the book
    void drawHud(Frame& f, int lines, int t) override;
    void drawEffects(Frame& f, const Tetris& g, int t) override;
private:
    // the intro (frames since the round started)
    static const int MOM_TALK0 = 6, MOM_TALK1 = 30, MOM_LEAVE = 32, DOOR_SHUT = 38, L0 = 62, B0 = 72, BOOK_FLY0 = 80, BOOK_FLY1 = 96;
    static const int G0 = 98, GOO1_0 = 102, GOO1_1 = 120, JB = 124, FIGHT1 = 138, DROP1 = 145, WG0 = 151, GOO2_1 = 167, S0 = 174, PLAY0 = 182;
    // the ending (frames since it started)
    static const int SHAKE2 = 16, SHAKE3 = 32, TENT_HIDE = 21, GOO_DRY = 37, DOOR_IN = 53;
    static const int TALK1 = 61, TALK2 = 83, TALK3 = 105, TALK_LEN = 20, KID_UP = 127, CRAWL1 = 143, RISE1 = 149, HOP1 = 157, LOOK1 = 163, KID_WALK0 = 165, KID_DOOR = 199;
    static const int ALL_GONE = 205, END_FADE = 216, END_LEN = 228;

    struct Stain { std::vector<std::pair<int, int>> cells; int born; };
    bool started_ = false;
    int start_ = 0, lastT_ = 0, lines_ = 0, clearT_ = -100, clearN_ = 1;
    int eyeT_ = -1, closT_ = -1, chestT_ = -1, endT_ = -1;
    std::vector<Stain> stains_;
    int nextStainT_ = -1, snatchT_ = -1, nextSnatchT_ = -1, snatchSrc_ = 0;
    int pieceX_ = 0, pieceY_ = 0;                  // where the falling piece is (screen px), for the tentacles
    unsigned rng_ = 12345;

    int f(int t) const { return t - start_; }                     // frames since the round started
    int e(int t) const { return endT_ >= 0 ? t - endT_ : -1; }    // frames since the ending started
    unsigned rnd() { rng_ = rng_ * 1103515245u + 12345u; return (rng_ >> 8) & 0xFFFFFF; }
    // the room
    void drawRoom(Frame& fr);
    void drawWindow(Frame& fr, int pf, int ee);
    void drawShelf(Frame& fr, int pf);
    void drawDoor(Frame& fr, int t);
    void drawCloset(Frame& fr, int t);
    void drawChest(Frame& fr, int t);
    void drawHoodie(Frame& fr, int t);
    void drawBed(Frame& fr, int t);
    void drawToys(Frame& fr);
    void drawSoldiers(Frame& fr, int t);
    void drawCocoon(Frame& fr, int t);
    void drawBoardGoo(Frame& fr, int t);
    void drawStains(Frame& fr, int t);
    void drawIntro(Frame& fr, int pf);
    void drawEnding(Frame& fr, int t);
    void drawTentacles(Frame& fr, int t);
    void drawMist(Frame& fr, int t, float k);
    void drawTentacle(Frame& fr, float bx, float by, float tx, float ty, int t, float w0, float w1, float curl, int seed);
    void drawMoonPatch(Frame& fr, float k);
    // how far along things are
    float tentOut(int t) const;                    // 0..1: tentacles out of the chest and from under the bed
    bool wolfOut(int t) const;
    bool dry(int t) const { return endT_ >= 0 && e(t) >= GOO_DRY; }
};