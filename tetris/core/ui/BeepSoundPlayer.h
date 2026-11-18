#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include "ui/ISoundPlayer.h"

// Windows Beep()로 효과음을 낸다. (Windows 전용, main.cpp에서만 생성)
// Beep는 소리가 끝날 때까지 멈추므로
//     별도 작업 스레드가 큐에 쌓인 음을 차례로 재생한다. (게임 루프는 멈추지 않는다)
class BeepSoundPlayer : public ISoundPlayer {
public:
    BeepSoundPlayer();
    ~BeepSoundPlayer() override;
    BeepSoundPlayer(const BeepSoundPlayer&) = delete;
    BeepSoundPlayer& operator=(const BeepSoundPlayer&) = delete;

    void play(SoundId id) override;
    void play(int frequencyHz, int durationMs) override;

private:
    struct Note {
        int frequencyHz;
        int durationMs;
    };
    void enqueue(Note note);
    void worker();

    std::deque<Note> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_wake;
    bool m_stop = false;
    std::thread m_thread;
};
