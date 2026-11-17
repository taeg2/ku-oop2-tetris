#pragma once
#include "ui/TextBoxView.h"
#include "flow/Scene.h"

// 스테이지 클리어 연출: 완성된 사진을 보여 준 뒤, 앨범의 빈 칸으로 날아가 붙는다. (기획안 7장)
class AlbumScene : public Scene {
public:
    AlbumScene(GameServices& services, int stageIndex);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    enum class Phase { Showcase, Insert, Caption };

    void renderShowcase(Console& console) const;
    void renderAlbum(Console& console) const;
    void goNext();

    int m_stageIndex;
    Phase m_phase = Phase::Showcase;
    int m_phaseMs = 0;
    TextBoxView m_caption;
};
