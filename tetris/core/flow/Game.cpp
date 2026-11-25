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
      m_sound(sound),
      m_album(config::kStageCount),
      m_story(m_catalog, m_sound, m_rng),
      m_services{m_sound, m_album, m_story, m_catalog, m_rng, m_scenes} {}

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
    if (m_shownSceneChange != m_scenes.changeCount()) {
        // 장면이 바뀌면 터미널을 한 번 지운다. 그림(Sixel)은 글자 버퍼 밖에 있어서 지우지 않으면 남는다.
        m_shownSceneChange = m_scenes.changeCount();
        m_console.clearScreen();
    }
    m_console.clear();
    m_scenes.render(m_console);
    m_console.present();
}
