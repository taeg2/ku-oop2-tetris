#include "flow/Clock.h"
#include <thread>

Clock::Clock() : m_start(std::chrono::steady_clock::now()) {}

int Clock::elapsedMs() const {
    const auto elapsed = std::chrono::steady_clock::now() - m_start;
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
}

void Clock::restart() {
    m_start = std::chrono::steady_clock::now();
}

void Clock::sleepMs(int ms) {
    if (ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}
