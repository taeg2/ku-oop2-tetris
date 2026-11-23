#pragma once
#include <string>
#include "ui/Command.h"
#include "ui/PixelArt.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 게임의 마지막 장면. 서재에서 앨범을 내려놓으면 아이들이 들어와 "이제 기분이 어때요?"라고 묻고, 그가 "나아졌어"라고 답한 뒤 끝난다
class EndingView : public View {
public:
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    explicit EndingView(Point origin);
    void start();
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool finished() const { return m_started >= 0 && m_t - m_started >= kEnd; }
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    static constexpr int kClose = 24, kPut = 34, kEnter = 44, kWalk = 30, kAsk = 80, kAnswer = 104, kHug = 128, kFade = 150, kEnd = 186;
    int m_t = 0;
    int m_accMs = 0;
    int m_started = -1;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
