#pragma once
#include "flow/NarrativeScene.h"

// 프롤로그 (도입). 아버지가 기억을 잃었다는 사실과 게임 목표를 알려 준다.
class PrologueScene : public NarrativeScene {
public:
    explicit PrologueScene(GameServices& services);

protected:
    void renderBackground(Console& console) const override;
    void onNarrationEnd() override;
};
