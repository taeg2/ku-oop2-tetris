#pragma once
#include <chrono>
using namespace std;

// 프레임 사이 경과 시간 측정
class Clock {
public:
    Clock();
    int elapsedMs() const;
    void restart();
    static void sleepMs(int ms);

private:
    chrono::steady_clock::time_point m_start;
};
