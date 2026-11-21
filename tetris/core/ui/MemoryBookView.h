#pragma once
#include <string>
#include <vector>
#include "ui/Command.h"
#include "ui/PixelArt.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 기억의 책. 지금까지 모은 기억(12칸)과 지금 하고 있는 라운드를 한 쪽에 보여 준다. 보기만 하고 라운드를 고르지는 않는다
class MemoryBookView : public View {
public:
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    static constexpr int kSlots = 12;
    explicit MemoryBookView(Point origin);
    void open(const std::vector<bool>& remembered, int current, const Frame& background);  // remembered: 12칸 (stage * 4 + 칸), current: 하고 있는 칸 (없으면 -1)
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool isOpen() const { return m_open; }
    int rememberedCount() const;
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    bool m_open = false;
    int m_sel = 0, m_current = -1, m_t = 0, m_accMs = 0, m_openT = 0;
    std::vector<bool> m_remembered = std::vector<bool>(kSlots, false);
    std::vector<RGB> m_background;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
