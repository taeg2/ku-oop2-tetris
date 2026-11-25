#pragma once
#include "flow/Scene.h"

// 엔딩 (ui의 EndingView). 서재에서 앨범을 내려놓으면 아이들이 들어와 "이제 기분이 어때요?"라고 묻고,
//     아버지가 "나아졌어"라고 답한 뒤 THE END. 끝난 뒤 Enter를 누르면 타이틀로 간다.
class EndingScene : public Scene {
public:
    explicit EndingScene(GameServices& services);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;
};
