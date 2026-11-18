// Game.cpp
#include "Game.h"
#include "Font.h"
#include <chrono>
#include <thread>

Game::Game() {
    rounds_.push_back(std::make_unique<LakeRound>());      // round 1: a breezy day at the lake
    rounds_.push_back(std::make_unique<KitchenRound>());   // round 2: dinner at the lake house
    rounds_.push_back(std::make_unique<BeachRound>());     // round 3: sand castles at sunset
    rounds_.push_back(std::make_unique<BedroomRound>());   // round 4: bedtime, the boss
}

void Game::jumpTo(int round) {
    current_ = round;
    tetris_.reset();
    rounds_[current_]->reset();
    t_ = 0; clearFrame_ = -1;
    renderer_.forceFullRedraw();
}

void Game::afterDrop(int cleared) {
    if (cleared > 0) clearFrame_ = 0;                     // rows flash for 3 frames, then collapse in update()
}

void Game::handle(Key k) {
    bool stopped = roundCleared() || tetris_.over() || tetris_.clearing() || !rounds_[current_]->playing(t_);
    if (rounds_[current_]->swapLeftRight()) {                      // round 4: the eye swaps left and right
        if (k == Key::Left) k = Key::Right; else if (k == Key::Right) k = Key::Left;
    }
    switch (k) {
    case Key::Left:     if (!stopped) tetris_.move(-1); break;
    case Key::Right:    if (!stopped) tetris_.move(1); break;
    case Key::Rotate:   if (!stopped) tetris_.rotate(); break;
    case Key::SoftDrop: if (!stopped) afterDrop(tetris_.step()); break;
    case Key::HardDrop: if (!stopped) afterDrop(tetris_.hardDrop()); break;
    case Key::Enter:
        if (roundCleared() && rounds_[current_]->endingDone()) jumpTo((current_ + 1) % (int)rounds_.size());
        else if (tetris_.over()) jumpTo(current_);
        break;
    case Key::Round1: jumpTo(current_); break;           // R: restart this round
    case Key::Jump1:  jumpTo(0); break;                  // 1 / 2 / 3: go straight to a round (for testing)
    case Key::Jump2:  if (rounds_.size() > 1) jumpTo(1); break;
    case Key::Jump3:  if (rounds_.size() > 2) jumpTo(2); break;
    case Key::Jump4:  if (rounds_.size() > 3) jumpTo(3); break;
    case Key::Event:  rounds_[current_]->specialEvent(t_); break;
    case Key::Pause:  paused_ = !paused_; break;
    case Key::Escape: running_ = false; break;
    default: break;
    }
}

void Game::update() {
    rounds_[current_]->control(tetris_, t_);
    rounds_[current_]->update(tetris_, t_);
    if (tetris_.clearing()) {
        if (++clearFrame_ >= 3) {
            std::vector<int> rows = tetris_.clearingRows();
            tetris_.collapse();
            clearFrame_ = -1;
            rounds_[current_]->onLinesCleared(rows, tetris_.lines(), t_);
        }
        return;
    }
    if (roundCleared() || tetris_.over() || !rounds_[current_]->playing(t_)) return;
    if (++gravity_ >= 5) { gravity_ = 0; afterDrop(tetris_.step()); }   // one row every 0.5 s
}

void Game::draw(Frame& f) {
    resetPalette();
    rounds_[current_]->draw(f, tetris_, t_, clearFrame_ >= 0 ? std::min(clearFrame_, 2) : -1);
    bool ownScreen = roundCleared() && rounds_[current_]->drawsOwnClearScreen();
    if ((roundCleared() && rounds_[current_]->endingDone() && !ownScreen) || tetris_.over()) {
        const char* msg = roundCleared() ? "ROUND CLEAR!" : "TRY AGAIN";
        f.rect(170, 118, 140, 34, C(30, 22, 30)); f.rect(172, 120, 136, 30, C(250, 236, 200));
        Font::center(f, msg, 240, 126, C(150, 58, 70));
        Font::center(f, "PRESS ENTER", 240, 138, C(120, 90, 70));
    }
}

int Game::pickScale() const {
    int cols, rows;
    Console::size(cols, rows);
    int s = std::min(cols * 10 / Frame::W, (rows - 1) * 20 / Frame::H);   // WT cell = 10x20 Sixel px
    return std::max(1, std::min(s, 4));
}

void Game::run() {
    using clock = std::chrono::steady_clock;
    renderer_.setScale(pickScale());
    double fps = 10;
    auto last = clock::now();
    while (running_) {
        auto start = clock::now();
        for (Key k = input_.poll(); k != Key::None; k = input_.poll()) handle(k);
        if (!paused_) update();
        if (t_ % 20 == 0) {                                   // window resized?
            int s = pickScale();
            if (s != renderer_.scale()) { renderer_.setScale(s); Console::write("\x1b[2J"); }
        }
        Frame f;
        draw(f);
        Console::write(renderer_.present(f));

        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - last).count();
        last = now;
        if (dt > 0) fps = fps * 0.9 + 0.1 / dt;
        if (t_ % 10 == 0)
            Console::setTitle(std::string("Stage 1 - ") + rounds_[current_]->name() + "  |  " + std::to_string((int)(fps + 0.5)) +
                              " fps  |  " + std::to_string(renderer_.lastBytes() / 1024) + " KB/frame  |  scale " + std::to_string(renderer_.scale()));
        if (!paused_) ++t_;
        auto wait = std::chrono::milliseconds(100) - (clock::now() - start);
        if (wait.count() > 0) std::this_thread::sleep_for(wait);
    }
}