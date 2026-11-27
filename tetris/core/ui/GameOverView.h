#pragma once
#include <string>
#include "ui/Command.h"
#include "ui/PixelArt.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
using namespace std;
class GameOverView : public View {
public:
    enum class Choice { None, Retry, Home };
    inline static const int kScale = 2;
    inline static const int kFrameMs = 100;
    inline static const int kInputLockFrames = 8;
    explicit GameOverView(Point origin);
    void open();
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool isOpen() const { return m_open; }
    bool hasChoice() const { return m_choice != Choice::None; }
    Choice takeChoice();
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    bool m_open = false;
    int m_t = 0;
    int m_accMs = 0;
    int m_openT = 0;
    int m_focus = 0;
    Choice m_choice = Choice::None;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable string m_sixel;
    SixelEncoder m_encoder;
};
