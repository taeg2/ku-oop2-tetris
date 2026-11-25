#pragma once
#include "flow/Scene.h"

// 앨범 장면 (ui의 AlbumView). 라운드를 끝낼 때마다 열려 방금 찾은 기억을 붙이고, 다음 라운드를 고르게 한다.
//     스테이지의 네 칸이 다 차면 앨범이 스스로 페이지를 넘기고 다음 집 장면으로, 마지막 스테이지면 엔딩으로 간다.
class AlbumScene : public Scene {
public:
    inline static const int kNoRoundCleared = -1;

    AlbumScene(GameServices& services, int stageIndex, int clearedRound);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

private:
    bool isStageDone() const;
    bool isLastStage() const;

    int m_stageIndex;
    int m_clearedRound;
    int m_timeMs = 0;
};
