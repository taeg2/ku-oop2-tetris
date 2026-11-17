#include "play/SoundCueListener.h"

void SoundCueListener::onEvent(const GameEvent& event) {
    using Type = GameEvent::Type;
    switch (event.type) {
        case Type::PieceLocked: m_sound.play(SoundId::Lock); break;
        case Type::LinesCleared: m_sound.play(SoundId::LineClear); break;
        case Type::PieceFilled: m_sound.play(SoundId::PieceFilled); break;
        case Type::PhotoHalf: m_sound.play(SoundId::BossAppear); break;
        case Type::PhotoComplete: m_sound.play(SoundId::StageClear); break;
        case Type::BossTriggered: m_sound.play(SoundId::BossTrigger); break;
        case Type::PillGained: m_sound.play(SoundId::PillGain); break;
        case Type::PillUsed: m_sound.play(SoundId::PillUse); break;
        case Type::StageFailed: m_sound.play(SoundId::GameOver); break;
    }
}
