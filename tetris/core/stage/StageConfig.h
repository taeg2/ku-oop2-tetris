#pragma once
#include <functional>
#include <string>
#include "boss/Boss.h"
#include "stage/PhotoArt.h"

// 스테이지 하나의 데이터. 스테이지끼리 다른 점은 클래스가 아니라 이 데이터로 표현한다.
//     보스는 재도전할 때마다 새로 만들어야 하므로 "만드는 함수"를 들고 있다.
struct StageConfig {
    std::string title;      // "STAGE 1"
    std::string period;     // "어린 시절"
    std::string blockTheme; // 블록 테마 설명
    PhotoArt photo;
    int baseDropMs;
    std::function<Boss()> makeBoss;
};
