#include "board/Cell.h"

void Cell::fill(Color color) {
    m_filled = true;
    m_color = color;
    m_pill.reset();
}

void Cell::fill(Color color, PillType pill) {
    fill(color);
    m_pill = pill;
}

void Cell::clear() {
    m_filled = false;
    m_color = Color::Black;
    m_pill.reset();
}
