#include "board/Cell.h"

void Cell::fill(TetrominoType type) {
    m_type = type;
    m_pill.reset();
}

void Cell::fill(TetrominoType type, PillType pill) {
    fill(type);
    m_pill = pill;
}

void Cell::clear() {
    m_type.reset();
    m_pill.reset();
}