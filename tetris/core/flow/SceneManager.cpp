#include "flow/SceneManager.h"
using namespace std;

void SceneManager::change(unique_ptr<Scene> next) {
    m_current = std::move(next);
    ++m_changeCount;
    if (m_current) {
        m_current->onEnter();
    }
    applyRequests();  // onEnter()에서 바로 전환을 요청했을 수도 있다
}

void SceneManager::request(unique_ptr<Scene> next) {
    m_pending = std::move(next);
}

void SceneManager::requestQuit() {
    m_quitRequested = true;
}

void SceneManager::handle(Command command) {
    if (m_current) {
        m_current->handle(command);
        applyRequests();
    }
}

void SceneManager::update(int dtMs) {
    if (m_current) {
        m_current->update(dtMs);
        applyRequests();
    }
}

void SceneManager::render(Console& console) const {
    if (m_current) {
        m_current->render(console);
    }
}

void SceneManager::applyRequests() {
    if (m_quitRequested) {
        m_quitRequested = false;
        m_pending.reset();
        m_current.reset();
        return;
    }
    if (m_pending) {
        change(std::move(m_pending));
    }
}
