#include "flow/Scene.h"
#include "flow/SceneManager.h"

void Scene::onEnter() {}

void Scene::changeTo(std::unique_ptr<Scene> next) {
    m_services.scenes.request(std::move(next));
}

void Scene::quit() {
    m_services.scenes.requestQuit();
}
