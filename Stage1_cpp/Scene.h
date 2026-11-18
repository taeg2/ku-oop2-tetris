// Scene.h - base class for every round. The Game only talks to Scene, never to a specific round.
#pragma once
#include "Art.h"
#include "Tetris.h"
#include "ToyBlocks.h"
#include <vector>

class Scene {
public:
    static const int GOAL = 10;                 // lines to clear a round (default)
    virtual int goal() const { return GOAL; }

    virtual ~Scene() = default;
    virtual const char* name() const = 0;
    virtual int boardX() const { return 100; }  // top-left of the 80x160 board
    virtual int boardY() const { return 60; }

    // one frame: background -> board -> round extras (NEXT holder, HUD, characters)
    // clearFrame: 0,1,2 while full rows flash before they disappear, -1 otherwise
    void draw(Frame& f, const Tetris& g, int t, int clearFrame = -1) {
        drawBackground(f, g, t);
        drawBoardFrame(f, t);
        ToyBlocks::board(f, g, boardX(), boardY(), playing(t));
        if (clearFrame >= 0) ToyBlocks::flashRows(f, g, boardX(), boardY(), clearFrame);
        drawForeground(f, g, t);
        drawNext(f, g.next(), t);
        drawHud(f, std::min(g.lines(), goal()), t);
        drawEffects(f, g, t);
    }
    virtual void reset() {}                                          // a new attempt at this round
    virtual void update(const Tetris& /*g*/, int /*t*/) {}            // per-frame logic: effects, events, cutscene
    virtual void onLinesCleared(const std::vector<int>& /*rows*/, int /*totalLines*/, int /*t*/) {}
    virtual bool endingDone() const { return true; }                 // the round's ending scene has finished
    virtual bool drawsOwnClearScreen() const { return false; }      // true: no generic ROUND CLEAR banner
    virtual void specialEvent(int /*t*/) {}                          // W key: trigger the round's event (for demos)
    virtual bool playing(int /*t*/) const { return true; }            // false during an intro: no input, no gravity
    virtual bool swapLeftRight() const { return false; }              // true: the Left and Right keys are swapped
    virtual void control(Tetris& /*g*/, int /*t*/) {}                 // rounds that change the rules (blocked cells, snatches)

protected:
    virtual void drawBackground(Frame& f, const Tetris& g, int t) = 0;
    virtual void drawBoardFrame(Frame& f, int t);                // wooden frame; rounds may override
    virtual void drawForeground(Frame& /*f*/, const Tetris& /*g*/, int /*t*/) {}
    virtual void drawNext(Frame& f, int piece, int t) = 0;       // teddy / pot / octopus / mist
    virtual void drawHud(Frame& f, int lines, int t) = 0;        // chalkboard / sticky note / bucket
    virtual void drawEffects(Frame& /*f*/, const Tetris& /*g*/, int /*t*/) {}   // flying blocks, sparkles, bubbles

    // shared UI: MENU and SETTING signs, Memory Book
    static void drawSign(Frame& f, int x, int y, int w, const char* label);
    static void drawMemoryBook(Frame& f, int x, int y);
    static void drawCommonUI(Frame& f);
};
