#pragma once

#include <vector>

#include "flow/Scene.h"
#include "stage/Album.h"
#include "stage/Stage.h"

// 스테이지를 클리어한 뒤, 완성한 사진이 앨범에 들어가는 장면.
// 시작할 때 사진을 앨범에 넣고, 이전 사진과 함께 보여 준다. Enter를 누르면 끝난다.
class AlbumScene : public Scene {
public:
    AlbumScene(Album& album, const Stage& stage);

    bool isFinished() const override;
    void render(Renderer& renderer) const override;

    void enter() override;
    void update(int elapsedMs, const std::vector<Action>& actions) override;

private:
    Album& album;
    const Stage& stage;
    int newPhotoIndex;
    bool finished;
};
