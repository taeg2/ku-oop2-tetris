#include "flow/PlayScene.h"

#include <algorithm>

PlayScene::PlayScene(StageSession& session, PlayField& field)
    : session(session), field(field), finished(false) {}

bool PlayScene::isFinished() const {
    return finished;
}

void PlayScene::render(Renderer& renderer) const {
    renderer.drawPlay(field, session);
}

void PlayScene::enter() {
    finished = false;
    session.start();
}

void PlayScene::update(int elapsedMs, const std::vector<Action>& actions) {
    if (session.isCleared()) {
        if (hasAction(actions, Action::Confirm)) {
            finished = true;
        }
        return;
    }
    if (session.isFailed()) {
        if (hasAction(actions, Action::Confirm)) {
            session.restart();
        }
        return;
    }

    for (Action action : actions) {
        handlePlayAction(action);
    }
    session.update(elapsedMs);
}

bool PlayScene::hasAction(const std::vector<Action>& actions, Action action) const {
    return std::find(actions.begin(), actions.end(), action) != actions.end();
}

void PlayScene::handlePlayAction(Action action) {
    // 입력 조작을 게임 커맨드로 옮기는 표다. (블록·보스 종류에 따른 분기가 아니다)
    switch (action) {
    case Action::MoveLeft:     field.moveLeft();         break;
    case Action::MoveRight:    field.moveRight();        break;
    case Action::SoftDrop:     field.softDrop();         break;
    case Action::HardDrop:     field.hardDrop();         break;
    case Action::Rotate:       field.rotate();           break;
    case Action::UseMedicine1: session.useMedicine(0);   break;
    case Action::UseMedicine2: session.useMedicine(1);   break;
    default:                                              break;
    }
}
