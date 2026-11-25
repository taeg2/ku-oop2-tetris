#include "flow/Clock.h"
#include <thread>
using namespace std;

Clock::Clock() : m_start(chrono::steady_clock::now()) {}

int Clock::elapsedMs() const {
    const auto elapsed = chrono::steady_clock::now() - m_start;
    return static_cast<int>(chrono::duration_cast<chrono::milliseconds>(elapsed).count());
}

void Clock::restart() {
    m_start = chrono::steady_clock::now();
}

void Clock::sleepMs(int ms) {
    if (ms > 0) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}
