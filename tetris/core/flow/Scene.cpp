#include "flow/Scene.h"
#include "common/GameConfig.h"
#include "flow/SceneManager.h"

void Scene::onEnter() {}

void Scene::changeTo(std::unique_ptr<Scene> next) {
    m_services.scenes.request(std::move(next));
}

void Scene::quit() {
    m_services.scenes.requestQuit();
}

void Scene::drawHelp(Console& console, const std::string& text) {
    console.drawCentered(layout::kHelpY, text, Color::DarkGray);
}

void Scene::drawDialog(Console& console, const Rect& area, Color frameColor) {
    console.fill(area, Color::Black);
    console.frame(area, frameColor);
}
