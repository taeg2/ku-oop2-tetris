#pragma once
#include <memory>
#include <string>
#include <vector>
#include "ui/AlbumScreen.h"
#include "ui/Command.h"
#include "ui/EmptyPlayState.h"
#include "ui/RoundScene.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
// 라운드를 고르는 앨범 화면. 라운드가 끝날 때마다 열리고, 라운드를 고르기 전에는 닫을 수 없다
// 스테이지를 다 깨면 스스로 페이지를 넘기고 stageFinished()가 true가 된다. 이후 PrologueView::start(새 stage)로 집 장면을 연다
class AlbumView : public View {
public:
    static constexpr int kScale = 2;
    static constexpr int kFrameMs = 100;
    explicit AlbumView(Point origin);
    void open(int stage, int clearedRound);  // clearedRound: 방금 깬 라운드 번호, 처음 열 때는 -1
    void handle(Command command);
    void update(int dtMs) override;
    void draw(Console& console) const override;
    bool isOpen() const { return m_album.isOpen(); }
    bool hasPick() const { return m_pick >= 0; }
    int takePick();  // 고른 라운드 번호(stage * 4 + 칸)를 돌려주고 비운다
    bool stageFinished() const { return m_stageFinished; }
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    AlbumScreen m_album;
    std::vector<std::unique_ptr<RoundScene>> m_previews;
    EmptyPlayState m_empty;
    int m_t = 0;
    int m_accMs = 0;
    int m_pick = -1;
    bool m_stageFinished = false;
    mutable int m_shown = -1;
    mutable Frame m_frame;
    mutable Frame m_preview;
    mutable int m_drawnT = -1;
    mutable std::string m_sixel;
    SixelEncoder m_encoder;
};
