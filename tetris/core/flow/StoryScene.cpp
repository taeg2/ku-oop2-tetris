#include "flow/StoryScene.h"

#include <algorithm>

StoryScene::StoryScene(const std::vector<std::string>& lines)
    : lines(lines), finished(false) {}

bool StoryScene::isFinished() const {
    return finished;
}

void StoryScene::render(Renderer& renderer) const {
    renderer.drawStory(lines);
}

void StoryScene::update(int /*elapsedMs*/, const std::vector<Action>& actions) {
    if (std::find(actions.begin(), actions.end(), Action::Confirm) != actions.end()) {
        finished = true;
    }
}
