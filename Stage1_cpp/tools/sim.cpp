// Linux-only: plays one round with a simple bot and saves every frame (not part of the game)
// build: g++ -std=c++17 -O2 -I.. sim.cpp ../Scene.cpp ../LakeRound.cpp ../KitchenRound.cpp ../BeachRound.cpp ../BedroomRound.cpp -o sim
// run:   ./sim <round 1..4> <max frames> [event frame] > log   (frames -> /tmp/r<round>sim.raw, 480x270 RGB)
#include "../LakeRound.h"
#include "../KitchenRound.h"
#include "../BeachRound.h"
#include "../BedroomRound.h"
#include <fstream>
#include <cstdio>
#include <memory>
#include <string>
static int score(const Tetris& g) {
    int h[10] = {}, holes = 0;
    for (int x = 0; x < 10; ++x) { bool seen = false; for (int y = 0; y < 20; ++y) { if (g.at(x, y) >= 0) { if (!seen) h[x] = 20 - y; seen = true; } else if (seen) ++holes; } }
    int agg = 0, bump = 0; for (int x = 0; x < 10; ++x) { agg += h[x]; if (x) bump += abs(h[x] - h[x - 1]); }
    return -agg * 5 - holes * 35 - bump * 2;
}
int main(int argc, char** argv) {
    srand(11);
    int round = atoi(argv[1]), frames = atoi(argv[2]), eventAt = argc > 3 ? atoi(argv[3]) : -1;
    std::unique_ptr<Scene> r;
    if (round == 1) r = std::make_unique<LakeRound>();
    else if (round == 2) r = std::make_unique<KitchenRound>();
    else if (round == 3) r = std::make_unique<BeachRound>();
    else r = std::make_unique<BedroomRound>();
    r->reset(); Tetris g;
    int clearFrame = -1, gravity = 0, maxPal = 0;
    std::ofstream out("/tmp/r" + std::to_string(round) + "sim.raw", std::ios::binary);
    int planX = -99, planR = 0, lastPy = 99;
    for (int t = 0; t < frames; ++t) {
        if (t == eventAt) r->specialEvent(t);
        r->control(g, t);
        r->update(g, t);
        if (g.clearing()) {
            if (++clearFrame >= 3) { auto rows = g.clearingRows(); g.collapse(); clearFrame = -1; r->onLinesCleared(rows, g.lines(), t); }
        } else if (g.lines() < r->goal() && !g.over() && r->playing(t)) {
            if (g.py() < lastPy) planX = -99;          // a new piece (or the old one was snatched)
            lastPy = g.py();
            if (g.py() <= 0 && planX == -99) {           // plan the best spot for this piece
                int best = -1 << 30;
                for (int rot = 0; rot < 4; ++rot) for (int x = -2; x < 10; ++x) {
                    Tetris c = g; for (int k = 0; k < rot; ++k) c.rotate();
                    for (int k = 0; k < 12; ++k) c.move(x < c.px() ? -1 : x > c.px() ? 1 : 0);
                    if (c.px() != x) continue;
                    int n = c.hardDrop(); if (n) { c.collapse(); }
                    int s = score(c) + n * n * 60;
                    if (s > best) { best = s; planX = x; planR = rot; }
                }
            }
            if (planX != -99 && t % 2 == 0) {            // act like a player: rotate, slide, then drop
                if (planR > 0) { g.rotate(); --planR; }
                else if (g.px() != planX) g.move(planX < g.px() ? -1 : 1);
                else { int n = g.hardDrop(); planX = -99; if (n) clearFrame = 0; }
            } else if (++gravity >= 5) { gravity = 0; int n = g.step(); if (n) clearFrame = 0; }
        }
        Frame f; resetPalette(); r->draw(f, g, t, clearFrame >= 0 ? std::min(clearFrame, 2) : -1);
        maxPal = std::max(maxPal, (int)palette().size());
        for (uint8_t i : f.px) { auto c = palette()[i]; out.put((char)c.r).put((char)c.g).put((char)c.b); }
        static int extra = 0;
        if (g.lines() >= r->goal() && r->endingDone() && ++extra > 20) { printf("done at frame %d\n", t); break; }
    }
    printf("lines %d  max palette %d\n", g.lines(), maxPal);
}
