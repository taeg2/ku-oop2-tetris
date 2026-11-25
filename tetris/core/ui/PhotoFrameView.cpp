#include "ui/PhotoFrameView.h"
#include "common/TextUtil.h"
#include "ui/PhotoPainter.h"

namespace {
const int kHighlightMs = 1400;
const int kQuadrantColumns = PhotoArt::kWidth;    // 12픽셀 x 2칸
const int kQuadrantRows = PhotoArt::kHeight / 2;  // 8줄
}  // namespace

PhotoFrameView::PhotoFrameView(Point origin, const StageSession& session) : View(origin), m_session(session) {}

void PhotoFrameView::update(int dtMs) {
    View::update(dtMs);
    const int pieces = m_session.progress().pieces();
    if (pieces > m_knownPieces) {
        m_highlightIndex = pieces - 1;
        m_highlightLeftMs = kHighlightMs;
    }
    m_knownPieces = pieces;
    if (m_highlightLeftMs > 0) {
        m_highlightLeftMs -= dtMs;
    }
}

Point PhotoFrameView::quadrantOrigin(int index) const {
    return {m_origin.x + 2 + (index % 2) * (kQuadrantColumns + 2), m_origin.y + 1 + (index / 2) * (kQuadrantRows + 1)};
}

Point PhotoFrameView::center() const {
    return {m_origin.x + kWidthColumns / 2, m_origin.y + kHeightRows / 2};
}

void PhotoFrameView::draw(Console& console) const {
    const PhotoArt& photo = m_session.config().photo;
    console.frame({m_origin.x, m_origin.y, kWidthColumns, kHeightRows}, Color::DarkYellow);
    console.fill({m_origin.x + 2, m_origin.y + 1, kWidthColumns - 4, kHeightRows - 2}, Color::White);

    const std::string title = "[ " + photo.title() + " ]";
    console.draw({m_origin.x + (kWidthColumns - TextUtil::width(title)) / 2, m_origin.y}, title, Color::Black,
                 Color::DarkYellow);

    if (m_session.progress().isComplete() && m_highlightLeftMs <= 0) {
        // 경계선이 사라지고 한 장의 사진이 된다 (아래 흰 띠는 사진 인화지 여백)
        PhotoPainter::paint(console, {m_origin.x + 3, m_origin.y + 1}, photo);
        return;
    }
    for (int i = 0; i < PhotoArt::kPieceCount; ++i) {
        drawQuadrant(console, i);
    }
}

void PhotoFrameView::drawQuadrant(Console& console, int index) const {
    const PhotoArt& photo = m_session.config().photo;
    const Point origin = quadrantOrigin(index);
    const Rect area = PhotoArt::quadrant(index);
    const bool revealed = index < m_session.progress().pieces();

    if (!revealed) {
        PhotoPainter::paint(console, origin, photo, area, PhotoTone::Hidden);
        console.draw({origin.x + kQuadrantColumns / 2 - 1, origin.y + kQuadrantRows / 2 - 1}, " ? ", Color::Gray,
                     Color::Black);
        return;
    }
    const bool flashing = index == m_highlightIndex && m_highlightLeftMs > 0 && !blink(120);
    PhotoPainter::paint(console, origin, photo, area, flashing ? PhotoTone::Faded : PhotoTone::Vivid);
}
