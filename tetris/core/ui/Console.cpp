#include "ui/Console.h"
#include <algorithm>
#include "common/TextUtil.h"

Console::Console(ITerminal& terminal, int width, int height)
    : m_terminal(terminal), m_width(width), m_height(height),
      m_back(static_cast<std::size_t>(width * height)), m_front(m_back.size()) {
    m_terminal.open(width, height);
}

Console::~Console() {
    m_terminal.close();
}

void Console::put(Point pos, char32_t ch, Color fg, Color bg) {
    if (pos.y < 0 || pos.y >= m_height || pos.x < 0 || pos.x >= m_width) {
        return;
    }
    const int w = TextUtil::charWidth(ch);
    if (w == 2 && pos.x + 1 >= m_width) {
        return;
    }
    // 2칸 문자의 절반을 덮어쓰면 남은 절반을 공백으로 정리한다.
    Glyph& target = glyphAt(pos.x, pos.y);
    if (target.continuation && pos.x > 0) {
        glyphAt(pos.x - 1, pos.y) = Glyph{U' ', fg, glyphAt(pos.x - 1, pos.y).bg, false};
    }
    const int end = pos.x + w;
    if (end < m_width && glyphAt(end, pos.y).continuation) {
        glyphAt(end, pos.y) = Glyph{U' ', fg, glyphAt(end, pos.y).bg, false};
    }
    target = Glyph{ch, fg, bg, false};
    if (w == 2) {
        glyphAt(pos.x + 1, pos.y) = Glyph{U' ', fg, bg, true};
    }
}

void Console::draw(Point pos, const std::string& text) {
    draw(pos, text, Color::Gray, Color::Black);
}

void Console::draw(Point pos, const std::string& text, Color fg, Color bg) {
    for (char32_t ch : TextUtil::decode(text)) {
        put(pos, ch, fg, bg);
        pos.x += TextUtil::charWidth(ch);
    }
}

void Console::draw(Point pos, const Cell& cell) {
    if (!cell.isFilled()) {
        draw(pos, "  ");
        return;
    }
    if (cell.pill()) {
        const Color pillColor = *cell.pill() == PillType::Blue ? Color::Blue : Color::Red;
        draw(pos, "()", pillColor, Color::White);
        return;
    }
    draw(pos, "  ", cell.color(), cell.color());
}

void Console::drawCentered(int y, const std::string& text, Color fg, Color bg) {
    const int x = (m_width - TextUtil::width(text)) / 2;
    draw({x, y}, text, fg, bg);
}

void Console::fill(const Rect& area, Color bg) {
    for (int y = area.y; y < area.bottom(); ++y) {
        for (int x = area.x; x < area.right(); ++x) {
            put({x, y}, U' ', Color::Gray, bg);
        }
    }
}

void Console::frame(const Rect& area, Color color) {
    fill({area.x, area.y, area.width, 1}, color);
    fill({area.x, area.bottom() - 1, area.width, 1}, color);
    fill({area.x, area.y, 2, area.height}, color);
    fill({area.right() - 2, area.y, 2, area.height}, color);
}

void Console::clear() {
    std::fill(m_back.begin(), m_back.end(), Glyph{});
}

bool Console::isRowChanged(int y) const {
    const auto begin = m_back.begin() + static_cast<std::ptrdiff_t>(y * m_width);
    return !std::equal(begin, begin + m_width, m_front.begin() + static_cast<std::ptrdiff_t>(y * m_width));
}

void Console::present() {
    for (int y = 0; y < m_height; ++y) {
        if (m_forceRedraw || isRowChanged(y)) {
            writeRow(y);
        }
    }
    m_terminal.flush();
    m_front = m_back;
    m_forceRedraw = false;
}

void Console::writeRow(int y) {
    // 마지막 줄의 마지막 칸에 쓰면 화면이 한 줄 밀리므로 비워 둔다.
    const int limit = (y == m_height - 1) ? m_width - 1 : m_width;
    int x = 0;
    while (x < limit) {
        const Glyph first = glyphAt(x, y);
        std::u32string run;
        int runEnd = x;
        while (runEnd < limit) {
            const Glyph& g = glyphAt(runEnd, y);
            if (g.fg != first.fg || g.bg != first.bg) {
                break;
            }
            if (!g.continuation) {
                run.push_back(g.ch);
            }
            ++runEnd;
        }
        m_terminal.write({x, y}, run, first.fg, first.bg);
        x = runEnd;
    }
}

void Console::clearScreen() {
    m_terminal.clearScreen();
    m_forceRedraw = true;
}
