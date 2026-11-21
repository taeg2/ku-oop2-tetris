#pragma once
#include <string>
#include <vector>
#include "ui/Command.h"
#include "ui/PixelArt.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 게임 중 Esc로 여는 메뉴. 멈춘 게임 화면을 어둡게 깔고 나무 표지판에 고를 것을 보여 준다
class MenuView : public View {
public:
    enum class Choice { None, Resume, MemoryBook, Settings, QuitToTitle };
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    explicit MenuView(Point origin);
    void open(const Frame& background);  // 게임 화면을 그린 바로 뒤에 부른다 (그 화면의 색을 읽는다)
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool isOpen() const { return m_open; }
    Choice choice() const { return m_choice; }
    void clearChoice();
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    static constexpr int kItems = 4;
    bool m_open = false;
    int m_sel = 0, m_t = 0, m_accMs = 0, m_openT = 0;
    Choice m_choice = Choice::None;
    std::vector<RGB> m_background;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
