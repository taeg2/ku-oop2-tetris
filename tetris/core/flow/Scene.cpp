#include "flow/Scene.h"
#include "flow/SceneManager.h"
using namespace std;

void Scene::onEnter() {}

void Scene::changeTo(unique_ptr<Scene> next) {
    m_services.scenes.request(std::move(next));
}

void Scene::quit() {
    m_services.scenes.requestQuit();
}
