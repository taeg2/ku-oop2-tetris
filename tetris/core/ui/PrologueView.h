#pragma once
#include <string>
#include "ui/Command.h"
#include "ui/HouseScene.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 집 오프닝. stage 0은 사탕, stage 1은 샌드위치를 먹고 서재에서 앨범을 집으면 finished()가 true가 된다
class PrologueView : public View {
public:
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    explicit PrologueView(Point origin);
    void start(int stage);
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool finished() const { return m_house.finished(); }
    int stage() const { return m_house.stage(); }
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    HouseScene m_house;
    int m_t = 0;
    int m_accMs = 0;
    mutable Frame m_frame;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
