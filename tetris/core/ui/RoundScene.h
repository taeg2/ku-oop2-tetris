#pragma once
#include <vector>
#include "ui/PixelArt.h"
#include "ui/PlayStateReader.h"
#include "ui/ToyBlocks.h"
// 라운드 하나의 그림. 새 라운드는 이 클래스를 상속하고 drawBackground/drawNext/drawHud를 구현한다
class RoundScene {
public:
    virtual ~RoundScene() = default;
    virtual const char* name() const = 0;
    virtual int boardX() const { return 100; }
    virtual int boardY() const { return 60; }
    virtual int goal() const { return 10; }  // 그림 속 진행 칸 수. 실제 줄 수는 StageArtView가 이 값으로 늘려 준다
    void draw(Frame& f, const PlayStateReader& g, int lines, int t, int clearFrame = -1) {
        m_state = &g;
        drawBackground(f, g, t);
        drawBoardFrame(f, t);
        ToyBlocks::board(f, g, boardX(), boardY(), playing(t));
        if (clearFrame >= 0) ToyBlocks::flashRows(f, g, boardX(), boardY(), clearFrame);
        drawForeground(f, g, t);
        drawNext(f, g.next(), t);
        drawHud(f, std::min(lines, goal()), t);
        drawEffects(f, g, t);
    }
    virtual void reset() {}
    virtual void update(const PlayStateReader&, int) {}
    virtual void onLinesCleared(const std::vector<int>&, int, int) {}
    virtual bool endingDone() const { return true; }  // 라운드 끝 연출(페이드)까지 끝났는가
    virtual bool playing(int) const { return true; }  // false면 연출 중이라 블록을 멈춰야 한다
protected:
    virtual void drawBackground(Frame& f, const PlayStateReader& g, int t) = 0;
    virtual void drawBoardFrame(Frame& f, int t);
    virtual void drawForeground(Frame&, const PlayStateReader&, int) {}
    virtual void drawNext(Frame& f, int piece, int t) = 0;
    virtual void drawHud(Frame& f, int lines, int t) = 0;
    virtual void drawEffects(Frame&, const PlayStateReader&, int) {}
    static void drawSign(Frame& f, int x, int y, int w, const char* label);
    static void drawMemoryBook(Frame& f, int x, int y);
    static void drawCommonUI(Frame& f);
    using ItemDraw = void (*)(Frame& f, int x, int y, int t);
    static void flyToBook(Frame& f, int m, int sx, int sy, ItemDraw item, const char* label, int t);  // m: 연출 시작 후 프레임, 26프레임 동안 sx,sy에서 앨범으로 날아간다
    const PlayStateReader& state() const { return *m_state; }
private:
    const PlayStateReader* m_state = nullptr;
};
