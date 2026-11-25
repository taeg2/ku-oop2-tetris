#pragma once
#include <string>
#include <vector>
#include "ui/Command.h"
#include "ui/PixelArt.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 설정 화면. 소리 켜기/끄기와 조작 키 목록을 보여 준다. 소리 값은 soundOn()으로 읽어 가서 바꿔 끼운다
class SettingsView : public View {
public:
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    explicit SettingsView(Point origin);
    void open(bool soundOn, const Frame& background);  // 뒤에 깔 화면을 그린 바로 뒤에 부른다
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool isOpen() const { return m_open; }
    bool soundOn() const { return m_sound; }
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    bool m_open = false, m_sound = true;
    int m_t = 0, m_accMs = 0, m_flipT = -100;
    std::vector<RGB> m_background;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
