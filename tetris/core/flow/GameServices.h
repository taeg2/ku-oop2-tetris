#pragma once
#include <random>
#include "flow/SwitchableSound.h"
using namespace std;

class Album;
class StoryArt;
class StageCatalog;
class SceneManager;

// 여러 장면이 함께 쓰는 공용 객체 묶음. Game이 소유하고 장면에는 참조로만 빌려준다.
class GameServices {
public:
    SwitchableSound& sound;  // 설정 화면에서 켜고 끌 수 있는 사운드 (ISoundPlayer)
    Album& album;
    StoryArt& story;         // 장면이 바뀌어도 이어지는 그림 화면과 진행 상태
    const StageCatalog& catalog;
    mt19937& rng;
    SceneManager& scenes;  // 장면 전환 요청 창구
};
