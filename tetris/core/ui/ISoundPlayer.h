#pragma once

// 효과음 종류
enum class SoundId {
    Lock, LineClear, PieceFilled, BossAppear, BossTrigger,
    PillGain, PillUse, StageClear, GameOver, Select
};

// 사운드 인터페이스. 실제 소리(BeepSoundPlayer)와 무음(MuteSoundPlayer)을 바꿔 끼운다.
class ISoundPlayer {
public:
    virtual ~ISoundPlayer() = default;
    virtual void play(SoundId id) = 0;                   // 정해진 효과음
    virtual void play(int frequencyHz, int durationMs) = 0;  // 음 하나
};
