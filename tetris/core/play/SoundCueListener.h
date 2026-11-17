#pragma once
#include "play/IGameEventListener.h"
#include "ui/ISoundPlayer.h"

// 게임 이벤트를 효과음으로 바꾼다. 로직 클래스가 사운드를 직접 부르지 않게 분리했다.
class SoundCueListener : public IGameEventListener {
public:
    explicit SoundCueListener(ISoundPlayer& sound) : m_sound(sound) {}
    void onEvent(const GameEvent& event) override;

private:
    ISoundPlayer& m_sound;
};
