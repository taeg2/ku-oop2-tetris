#include "stage/PhotoProgress.h"
#include "common/GameConfig.h"

PhotoProgress::PhotoProgress(EventBus& bus) : m_bus(bus) {}

void PhotoProgress::addLines(int count) {
    if (isComplete() || count <= 0) {
        return;
    }
    m_lines += count;
    while (m_lines >= config::kLinesPerPiece && !isComplete()) {
        m_lines -= config::kLinesPerPiece;
        ++m_pieces;
        m_bus.publish(GameEvent::Type::PieceFilled, m_pieces);
        if (m_pieces == config::kBossAppearPieces) {
            m_bus.publish(GameEvent::Type::PhotoHalf);
        }
        if (isComplete()) {
            m_bus.publish(GameEvent::Type::PhotoComplete);
        }
    }
}

int PhotoProgress::linesUntilNextPiece() const {
    return isComplete() ? 0 : config::kLinesPerPiece - m_lines;
}

bool PhotoProgress::isHalf() const {
    return m_pieces >= config::kBossAppearPieces;
}

bool PhotoProgress::isComplete() const {
    return m_pieces >= config::kPhotoPieces;
}

void PhotoProgress::reset() {
    m_lines = 0;
    m_pieces = 0;
}
