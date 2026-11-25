#pragma once
#include <random>
#include "flow/Clock.h"
#include "flow/GameServices.h"
#include "flow/SceneManager.h"
#include "flow/StoryArt.h"
#include "flow/SwitchableSound.h"
#include "stage/Album.h"
#include "stage/StageCatalog.h"
#include "ui/Console.h"
#include "ui/IInputSource.h"
#include "ui/ISoundPlayer.h"
#include "ui/ITerminal.h"

// 게임 전체의 뿌리. 터미널·입력·사운드는 추상 타입으로 받고(DIP, 구체 객체는 main.cpp가 만든다),
// 화면 버퍼·스테이지 데이터·앨범·장면을 소유하며 메인 루프(입력 -> update -> render)를 돈다.
class Game {
public:
    Game(ITerminal& terminal, IInputSource& input, ISoundPlayer& sound);
    void run();
    int exitCode() const { return m_exitCode; }

private:
    void tick(int dtMs);

    std::mt19937 m_rng;
    Console m_console;
    IInputSource& m_input;
    SwitchableSound m_sound;
    StageCatalog m_catalog;
    Album m_album;
    StoryArt m_story;
    SceneManager m_scenes;
    GameServices m_services;
    Clock m_clock;
    int m_exitCode = 0;
    int m_shownSceneChange = -1;
};
