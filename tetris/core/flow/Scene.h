#pragma once

#include <vector>

#include "ui/Action.h"
#include "ui/Renderer.h"

// 게임의 한 장면(프롤로그 자막, 스테이지 플레이, 앨범 연출, 엔딩)의 추상 클래스.
// Game은 장면 목록을 순서대로 진행할 뿐 장면의 종류를 모른다.
// 새 장면은 이 클래스를 상속한 클래스를 추가하고 main의 장면 목록에 넣으면 된다. (OCP)
class Scene {
public:
    virtual ~Scene();

    // ===== 쿼리 =====

    // 이 장면이 끝나 다음 장면으로 넘어가야 하는지
    virtual bool isFinished() const = 0;

    // 지금 장면을 그린다. 장면의 상태는 바꾸지 않는다.
    virtual void render(Renderer& renderer) const = 0;

    // ===== 커맨드 =====

    // 이 장면이 시작될 때 한 번 불린다. 기본은 아무 일도 하지 않는다.
    virtual void enter();

    // 시간이 elapsedMs만큼 흘렀고, 그동안 플레이어가 actions를 입력했음을 알린다.
    virtual void update(int elapsedMs, const std::vector<Action>& actions) = 0;
};
