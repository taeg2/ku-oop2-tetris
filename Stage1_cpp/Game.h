// Game.h - owns the rounds, the Tetris rules, the renderer and the input. Talks to rounds only through Scene.
#pragma once
#include "LakeRound.h"
#include "KitchenRound.h"
#include "BeachRound.h"
#include "BedroomRound.h"
#include "Renderer.h"
#include "Console.h"
#include <memory>
#include <vector>

class Game {
public:
    Game();
    void run();                      // main loop, 10 frames per second
    void jumpTo(int round);          // restart a round

private:
    std::vector<std::unique_ptr<Scene>> rounds_;
    int current_ = 0;
    Tetris tetris_;
    Renderer renderer_;
    InputHandler input_;
    int t_ = 0, gravity_ = 0, clearFrame_ = -1;
    bool running_ = true, paused_ = false;

    void handle(Key k);
    void update();
    void draw(Frame& f);
    bool roundCleared() const { return tetris_.lines() >= rounds_[current_]->goal(); }
    void afterDrop(int cleared);
    int pickScale() const;
};