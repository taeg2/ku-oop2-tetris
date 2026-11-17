#include "ui/View.h"

View::View(Point origin) : m_origin(origin) {}

void View::update(int dtMs) {
    m_timeMs += dtMs;
}

void View::setOrigin(Point origin) {
    m_origin = origin;
}

Point View::origin() const {
    return m_origin;
}

bool View::blink(int periodMs) const {
    return (m_timeMs / periodMs) % 2 == 0;
}
