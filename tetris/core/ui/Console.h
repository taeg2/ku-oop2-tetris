#pragma once
#include <string>
#include <vector>
#include "board/Cell.h"
#include "common/Color.h"
#include "common/Point.h"
#include "common/Rect.h"
#include "ui/ITerminal.h"

// 화면 한 칸 (문자 + 글자색 + 배경색). 한글처럼 2칸짜리 문자는 오른쪽 칸을 continuation으로 둔다.
struct Glyph {
    char32_t ch = U' ';
    Color fg = Color::Gray;
    Color bg = Color::Black;
    bool continuation = false;

    bool operator==(const Glyph& rhs) const {
        return ch == rhs.ch && fg == rhs.fg && bg == rhs.bg && continuation == rhs.continuation;
    }
};

// 화면 버퍼. 모든 그리기는 백 버퍼에 하고, present()에서 바뀐 줄만 ITerminal로 내보낸다.
// 그래서 화면 깜빡임이 없고, 운영체제별 출력 방법은 ITerminal 구현이 맡는다.
class Console {
public:
    Console(ITerminal& terminal, int width, int height);
    ~Console();
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    void draw(Point pos, const std::string& text);
    void draw(Point pos, const std::string& text, Color fg, Color bg = Color::Black);
    void draw(Point pos, const Cell& cell);  // 보드 한 칸 = 화면 2칸
    void drawCentered(int y, const std::string& text, Color fg, Color bg = Color::Black);
    void fill(const Rect& area, Color bg);
    void frame(const Rect& area, Color color);  // 테두리를 색칠된 칸으로 그린다
    void drawImage(Point pos, const std::string& sixel);  // Sixel 이미지를 pos 칸에 그린다. 글자 버퍼와 따로 present()에서 출력된다

    void clear();
    void present();      // 바뀐 줄만 터미널에 반영
    void clearScreen();  // 터미널 화면 전체 지우기

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    void put(Point pos, char32_t ch, Color fg, Color bg);
    Glyph& glyphAt(int x, int y) { return m_back[static_cast<std::size_t>(y * m_width + x)]; }
    bool isRowChanged(int y) const;
    void writeRow(int y);

    ITerminal& m_terminal;
    int m_width;
    int m_height;
    std::vector<Glyph> m_back;
    std::vector<Glyph> m_front;
    bool m_forceRedraw = true;
    std::vector<std::pair<Point, std::string>> m_images;
};
