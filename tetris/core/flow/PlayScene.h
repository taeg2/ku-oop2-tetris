#pragma once

#include <vector>

#include "flow/Scene.h"
#include "play/PlayField.h"
#include "stage/StageSession.h"

// 스테이지 하나를 플레이하는 장면.
// 플레이어의 조작을 PlayField·StageSession의 커맨드로 바꿔 전달한다.
// - 플레이 중: 이동·회전·낙하·약 사용
// - 실패: Enter를 누르면 스테이지 시작 상태로 재도전
// - 클리어: Enter를 누르면 장면이 끝난다 (다음은 앨범 장면)
class PlayScene : public Scene {
public:
    PlayScene(StageSession& session, PlayField& field);

    bool isFinished() const override;
    void render(Renderer& renderer) const override;

    // 스테이지를 시작한다. (이전 스테이지에서 가져온 약을 시작 상태로 기억한다)
    void enter() override;
    void update(int elapsedMs, const std::vector<Action>& actions) override;

private:
    bool hasAction(const std::vector<Action>& actions, Action action) const;

    // 플레이 중일 때 조작 하나를 처리한다.
    void handlePlayAction(Action action);

    StageSession& session;
    PlayField& field;
    bool finished;
};
