#ifdef _WIN32
// windows.h를 가장 먼저 include한다 (using namespace std; 뒤에 오면 byte 같은 이름이 겹칠 수 있다)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "ui/BeepSoundPlayer.h"
#include <map>
#include <vector>
using namespace std;

namespace {
const size_t kMaxQueuedNotes = 24;  // 너무 밀리면 버린다 (소리가 화면보다 늦지 않게)

// 주파수 0 = 쉼표
const map<SoundId, vector<pair<int, int>>> kMelodies = {
    {SoundId::Lock, {{196, 25}}},
    {SoundId::LineClear, {{523, 45}, {659, 45}, {784, 70}}},
    {SoundId::PieceFilled, {{523, 80}, {659, 80}, {784, 80}, {1047, 160}}},
    {SoundId::BossAppear, {{220, 160}, {0, 40}, {196, 160}, {0, 40}, {165, 320}}},
    {SoundId::BossTrigger, {{330, 60}, {247, 90}}},
    {SoundId::PillGain, {{988, 50}, {1319, 80}}},
    {SoundId::PillUse, {{880, 60}, {1175, 60}, {1568, 90}}},
    {SoundId::StageClear, {{523, 110}, {659, 110}, {784, 110}, {1047, 160}, {784, 110}, {1047, 300}}},
    {SoundId::GameOver, {{392, 200}, {330, 200}, {262, 420}}},
    {SoundId::Select, {{660, 30}}},
};
}  // namespace

BeepSoundPlayer::BeepSoundPlayer() : m_thread(&BeepSoundPlayer::worker, this) {}

BeepSoundPlayer::~BeepSoundPlayer() {
    {
        lock_guard<mutex> lock(m_mutex);
        m_stop = true;
        m_queue.clear();
    }
    m_wake.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void BeepSoundPlayer::play(SoundId id) {
    const auto it = kMelodies.find(id);
    if (it == kMelodies.end()) {
        return;
    }
    for (const auto& [frequency, duration] : it->second) {
        enqueue({frequency, duration});
    }
}

void BeepSoundPlayer::play(int frequencyHz, int durationMs) {
    enqueue({frequencyHz, durationMs});
}

void BeepSoundPlayer::enqueue(Note note) {
    {
        lock_guard<mutex> lock(m_mutex);
        if (m_queue.size() >= kMaxQueuedNotes) {
            return;
        }
        m_queue.push_back(note);
    }
    m_wake.notify_one();
}

void BeepSoundPlayer::worker() {
    while (true) {
        Note note{};
        {
            unique_lock<mutex> lock(m_mutex);
            m_wake.wait(lock, [this] { return m_stop || !m_queue.empty(); });
            if (m_stop) {
                return;
            }
            note = m_queue.front();
            m_queue.pop_front();
        }
        if (note.frequencyHz <= 0) {
            Sleep(static_cast<DWORD>(note.durationMs));
        } else {
            Beep(static_cast<DWORD>(note.frequencyHz), static_cast<DWORD>(note.durationMs));
        }
    }
}
#endif  // _WIN32
