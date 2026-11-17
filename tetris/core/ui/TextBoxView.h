#pragma once
#include <string>
#include "ui/View.h"

// 대사·자막 상자. 글자가 한 자씩 나오는 타자기 효과와 한글 폭을 고려한 줄바꿈을 지원한다.
class TextBoxView : public View {
public:
    TextBoxView(Point origin, int width, int height);

    void setText(const std::string& text);
    void setText(const std::string& speaker, const std::string& text);
    void setFramed(bool framed, Color frameColor = Color::DarkGray);
    void setTypingSpeed(int charsPerSecond);  // 0이면 한 번에 표시
    void advance(int chars);
    void showAll();
    bool isFullyShown() const;

    void update(int dtMs) override;
    void draw(Console& console) const override;

private:
    int innerWidth() const;

    int m_width;
    int m_height;
    std::string m_speaker;
    std::u32string m_text;
    std::size_t m_shown = 0;
    int m_charsPerSecond = 0;
    int m_typingMs = 0;
    bool m_framed = false;
    Color m_frameColor = Color::DarkGray;
};
