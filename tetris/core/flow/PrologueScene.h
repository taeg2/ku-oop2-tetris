#pragma once
#include "flow/Scene.h"

// 집 장면 (ui의 PrologueView). stage 0은 노인이 약을 먹고 아이로 깨어나는 오프닝,
//     stage 1·2는 스테이지 사이 쉬는 장면(샌드위치, 햄버거). 서재에서 앨범을 집으면 앨범 장면으로 넘어간다.
class PrologueScene : public Scene {
public:
    PrologueScene(GameServices& services, int stageIndex);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    int m_stageIndex;
};
