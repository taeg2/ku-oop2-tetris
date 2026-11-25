#include "ui/PrologueView.h"
#include <algorithm>
using namespace std;

PrologueView::PrologueView(Point origin) : View(origin) {}
void PrologueView::start(int stage) { m_house.start(stage, m_t); }
void PrologueView::handle(Command command) { m_house.handle(command, m_t); }
void PrologueView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) { m_accMs -= kFrameMs; ++m_t; m_house.update(m_t); }
}
void PrologueView::render() const {
    resetPalette();
    fill(m_frame.px.begin(), m_frame.px.end(), 0);
    m_house.draw(m_frame, m_t);
}
void PrologueView::draw(Console& console) const {
    if (m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
