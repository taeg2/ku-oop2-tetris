#include "flow/Game.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>

namespace {
    // 창을 옮기는 등으로 한 프레임이 너무 길어져도 블록이 한꺼번에 떨어지지 않게 막는 최대 시간
    const int maxElapsedMs = 100;
}

Game::Game(Renderer& renderer, InputSource& input, std::vector<std::unique_ptr<Scene>> scenes, int frameMs)
    : renderer(renderer),
      input(input),
      scenes(std::move(scenes)),
      frameMs(frameMs),
      currentIndex(0),
      quitRequested(false),
      started(false) {}

bool Game::isOver() const {
    return quitRequested || currentIndex >= static_cast<int>(scenes.size());
}

void Game::run() {
    using Clock = std::chrono::steady_clock;
    Clock::time_point previous = Clock::now();

    while (!isOver()) {
        const Clock::time_point now = Clock::now();
        const int elapsedMs = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now - previous).count());
        previous = now;

        step(std::min(elapsedMs, maxElapsedMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(frameMs));
    }
}

void Game::step(int elapsedMs) {
    if (isOver()) {
        return;
    }
    if (!started) {
        started = true;
        scenes[currentIndex]->enter();
    }

    input.poll();
    const std::vector<Action> actions = input.getActions();
    if (hasQuitAction(actions)) {
        quitRequested = true;
        return;
    }

    Scene& scene = *scenes[currentIndex];
    scene.update(elapsedMs, actions);
    if (scene.isFinished()) {
        moveToNextScene();
    }
    if (isOver()) {
        return;
    }

    renderer.beginFrame();
    scenes[currentIndex]->render(renderer);
    renderer.endFrame();
}

bool Game::hasQuitAction(const std::vector<Action>& actions) const {
    return std::find(actions.begin(), actions.end(), Action::Quit) != actions.end();
}

void Game::moveToNextScene() {
    ++currentIndex;
    if (!isOver()) {
        scenes[currentIndex]->enter();
    }
}
