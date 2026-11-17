#pragma once
#include <array>
#include "block/Tetromino.h"
#include "flow/Scene.h"

// 타이틀 화면. 원본 C 코드의 로고 화면을 옮긴 것으로, 테트리스 로고와 장식 블록이 바뀐다.
class TitleScene : public Scene {
public:
    explicit TitleScene(GameServices& services);
    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    void shuffleDecorations();

    std::array<Tetromino, 4> m_decorations;
    int m_timeMs = 0;
    int m_shuffleMs = 0;
};
