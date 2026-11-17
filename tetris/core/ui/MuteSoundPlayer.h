#pragma once
#include "ui/ISoundPlayer.h"

// 아무 소리도 내지 않는 사운드 (실행 인자 --mute, Mac/Linux, 테스트용)
class MuteSoundPlayer : public ISoundPlayer {
public:
    void play(SoundId id) override;
    void play(int frequencyHz, int durationMs) override;
};
