#pragma once
#include <memory>
#include "flow/Scene.h"

// 현재 장면 하나를 소유하고 입력·갱신·그리기를 넘긴다.
// 장면이 요청한 전환은 장면의 함수가 끝난 뒤에 적용한다 (실행 중인 장면을 지우지 않기 위해).
class SceneManager {
public:
    void change(std::unique_ptr<Scene> next);
    void request(std::unique_ptr<Scene> next);
    void requestQuit();

    void handle(Command command);
    void update(int dtMs);
    void render(Console& console) const;
    bool isRunning() const { return m_current != nullptr; }

private:
    void applyRequests();

    std::unique_ptr<Scene> m_current;
    std::unique_ptr<Scene> m_pending;
    bool m_quitRequested = false;
};
