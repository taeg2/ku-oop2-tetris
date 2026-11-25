#pragma once
#include "ui/ISoundPlayer.h"

// 설정 화면에서 소리를 켜고 끌 수 있게 실제 사운드를 감싼다. (데코레이터)
//     main.cpp가 만든 구체 사운드(BeepSoundPlayer/MuteSoundPlayer)는 그대로 두고, 꺼져 있으면 전달하지 않는다.
class SwitchableSound : public ISoundPlayer {
public:
    explicit SwitchableSound(ISoundPlayer& target);

    void play(SoundId id) override;
    void play(int frequencyHz, int durationMs) override;

    void setOn(bool on);
    bool isOn() const { return m_on; }

private:
    ISoundPlayer& m_target;
    bool m_on = true;
};
