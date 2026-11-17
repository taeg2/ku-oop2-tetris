#pragma once

#include <memory>
#include <vector>

#include "flow/Scene.h"
#include "ui/InputSource.h"
#include "ui/Renderer.h"

// 게임 루프. 장면 목록을 순서대로 진행한다.
// 매 프레임 "입력 모으기 → 장면 진행 → 화면 그리기"를 반복한다.
// 화면·입력·장면은 모두 추상 클래스로 받으므로, 콘솔을 그래픽으로 바꾸거나
// 장면을 추가해도 이 클래스는 고치지 않는다. (DIP, OCP)
class Game {
public:
    // scenes: 진행할 장면 순서 (main에서 만든다)
    // frameMs: 한 프레임의 길이
    Game(Renderer& renderer, InputSource& input, std::vector<std::unique_ptr<Scene>> scenes, int frameMs = 33);

    // ===== 쿼리 =====

    // 마지막 장면까지 끝났거나, 플레이어가 그만두었는지
    bool isOver() const;

    // ===== 커맨드 =====

    // 게임이 끝날 때까지 프레임을 반복한다. 실제 시간을 재서 step()에 넘긴다.
    void run();

    // 한 프레임을 진행한다. 시간이 elapsedMs만큼 흘렀다고 본다.
    void step(int elapsedMs);

private:
    bool hasQuitAction(const std::vector<Action>& actions) const;

    // 다음 장면으로 넘어가고, 그 장면을 시작한다.
    void moveToNextScene();

    Renderer& renderer;
    InputSource& input;
    std::vector<std::unique_ptr<Scene>> scenes;
    int frameMs;
    int currentIndex;
    bool quitRequested;
    bool started;
};
