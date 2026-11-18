#pragma once
#include <random>

class ISoundPlayer;
class Album;
class StageCatalog;
class SceneManager;

// 여러 장면이 함께 쓰는 공용 객체 묶음. Game이 소유하고 장면에는 참조로만 빌려준다.
struct GameServices {
    ISoundPlayer& sound;
    Album& album;
    const StageCatalog& catalog;
    std::mt19937& rng;
    SceneManager& scenes;  // 장면 전환 요청 창구
};
