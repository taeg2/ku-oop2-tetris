#pragma once
#include <random>

class Board;
class Tetromino;
class SpeedController;
class PhotoProgress;
class EventBus;

// 보스 기믹과 약이 "무엇을 바꿀 수 있는지"를 한 묶음으로 전달한다.
//     StageSession이 매번 만들어 넘기므로, 기믹·약은 StageSession 전체를 알 필요가 없다.
struct PlayContext {
    Board& board;
    Tetromino& current;
    SpeedController& speed;
    PhotoProgress& progress;
    EventBus& bus;
    std::mt19937& rng;
};
