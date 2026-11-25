#include "flow/SwitchableSound.h"

SwitchableSound::SwitchableSound(ISoundPlayer& target) : m_target(target) {}

void SwitchableSound::play(SoundId id) {
    if (m_on) {
        m_target.play(id);
    }
}

void SwitchableSound::play(int frequencyHz, int durationMs) {
    if (m_on) {
        m_target.play(frequencyHz, durationMs);
    }
}

void SwitchableSound::setOn(bool on) {
    m_on = on;
}
