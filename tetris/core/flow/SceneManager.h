#pragma once
#include <memory>
#include "flow/Scene.h"
using namespace std;

// 현재 장면 하나를 소유하고 입력·갱신·그리기를 넘긴다.
// 장면이 요청한 전환은 장면의 함수가 끝난 뒤에 적용한다 (실행 중인 장면을 지우지 않기 위해).
class SceneManager {
public:
    void change(unique_ptr<Scene> next);
    void request(unique_ptr<Scene> next);
    void requestQuit();

    void handle(Command command);
    void update(int dtMs);
    void render(Console& console) const;
    bool isRunning() const { return m_current != nullptr; }
    int changeCount() const { return m_changeCount; }  // 장면이 바뀔 때마다 1씩 는다 (화면 지우기 판단용)

private:
    void applyRequests();

    unique_ptr<Scene> m_current;
    unique_ptr<Scene> m_pending;
    bool m_quitRequested = false;
    int m_changeCount = 0;
};
