#include "ui/TextBoxView.h"
#include <algorithm>
#include "common/TextUtil.h"

TextBoxView::TextBoxView(Point origin, int width, int height) : View(origin), m_width(width), m_height(height) {}

void TextBoxView::setText(const std::string& text) {
    setText("", text);
}

void TextBoxView::setText(const std::string& speaker, const std::string& text) {
    m_speaker = speaker;
    m_text = TextUtil::decode(text);
    m_shown = m_charsPerSecond > 0 ? 0 : m_text.size();
    m_typingMs = 0;
}

void TextBoxView::setFramed(bool framed, Color frameColor) {
    m_framed = framed;
    m_frameColor = frameColor;
}

void TextBoxView::setTypingSpeed(int charsPerSecond) {
    m_charsPerSecond = charsPerSecond;
}

void TextBoxView::advance(int chars) {
    m_shown = std::min(m_text.size(), m_shown + static_cast<std::size_t>(chars));
}

void TextBoxView::showAll() {
    m_shown = m_text.size();
}

bool TextBoxView::isFullyShown() const {
    return m_shown >= m_text.size();
}

void TextBoxView::update(int dtMs) {
    View::update(dtMs);
    if (m_charsPerSecond <= 0 || isFullyShown()) {
        return;
    }
    m_typingMs += dtMs;
    const int msPerChar = 1000 / m_charsPerSecond;
    while (m_typingMs >= msPerChar && !isFullyShown()) {
        m_typingMs -= msPerChar;
        advance(1);
    }
}

int TextBoxView::innerWidth() const {
    return m_framed ? m_width - 6 : m_width;
}

void TextBoxView::draw(Console& console) const {
    Point pos = m_origin;
    int rows = m_height;
    if (m_framed) {
        console.fill({m_origin.x, m_origin.y, m_width, m_height}, Color::Black);
        console.frame({m_origin.x, m_origin.y, m_width, m_height}, m_frameColor);
        pos = {m_origin.x + 3, m_origin.y + 1};
        rows = m_height - 2;
    }
    if (!m_speaker.empty()) {
        console.draw(pos, m_speaker, Color::Yellow);
        ++pos.y;
        --rows;
    }

    std::size_t remaining = m_shown;
    const auto lines = TextUtil::wrap(m_text, innerWidth());
    for (std::size_t i = 0; i < lines.size() && static_cast<int>(i) < rows && remaining > 0; ++i) {
        const std::u32string visible = lines[i].substr(0, remaining);
        console.draw({pos.x, pos.y + static_cast<int>(i)}, TextUtil::encode(visible), Color::White);
        remaining -= std::min(remaining, lines[i].size() + 1);  // +1: 줄바꿈된 공백
    }
    if (isFullyShown() && m_framed && blink(400)) {
        console.draw({m_origin.x + m_width - 6, m_origin.y + m_height - 2}, "v", Color::Gray);
    }
}
