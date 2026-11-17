#pragma once

#include <string>
#include <vector>

#include "flow/Scene.h"

// 자막 장면. 프롤로그, 스테이지 소개, 엔딩처럼 문장 여러 줄을 보여 주고 Enter를 누르면 끝난다.
class StoryScene : public Scene {
public:
    explicit StoryScene(const std::vector<std::string>& lines);

    bool isFinished() const override;
    void render(Renderer& renderer) const override;

    void update(int elapsedMs, const std::vector<Action>& actions) override;

private:
    std::vector<std::string> lines;
    bool finished;
};
