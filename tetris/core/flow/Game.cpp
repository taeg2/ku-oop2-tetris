#include "flow/Game.h"
#include <algorithm>
#include <iostream>
#include "common/GameConfig.h"
#include "common/GameException.h"
#include "flow/TitleScene.h"

Game::Game(ITerminal& terminal, IInputSource& input, ISoundPlayer& sound)
    : m_rng(std::random_device{}()),
      m_console(terminal, config::kScreenWidth, config::kScreenHeight),
      m_input(input),
      m_album(config::kStageCount),
      m_services{sound, m_album, m_catalog, m_rng, m_scenes} {}

void Game::run() {
    try {
        m_scenes.change(std::make_unique<TitleScene>(m_services));
        m_clock.restart();
        while (m_scenes.isRunning()) {
            const int dt = std::min(m_clock.elapsedMs(), config::kMaxFrameDeltaMs);
            m_clock.restart();
            tick(dt);
            Clock::sleepMs(config::kFrameMs - m_clock.elapsedMs());
        }
        m_exitCode = 0;
    } catch (const GameException& e) {
        m_console.clearScreen();
        std::cerr << "[게임 오류] " << e.what() << std::endl;
        m_exitCode = 1;
    } catch (const std::exception& e) {
        m_console.clearScreen();
        std::cerr << "[예상하지 못한 오류] " << e.what() << std::endl;
        m_exitCode = 1;
    }
}

void Game::tick(int dtMs) {
    m_input.update();
    while (m_input.hasCommand() && m_scenes.isRunning()) {
        const Command command = m_input.peekCommand();
        m_input.popCommand();
        m_scenes.handle(command);
    }
    if (!m_scenes.isRunning()) {
        return;
    }
    m_scenes.update(dtMs);
    if (!m_scenes.isRunning()) {
        return;
    }
    m_console.clear();
    m_scenes.render(m_console);
    m_console.present();
}
