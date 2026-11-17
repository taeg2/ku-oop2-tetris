#include "flow/AlbumScene.h"

#include <algorithm>

AlbumScene::AlbumScene(Album& album, const Stage& stage)
    : album(album), stage(stage), newPhotoIndex(-1), finished(false) {}

bool AlbumScene::isFinished() const {
    return finished;
}

void AlbumScene::render(Renderer& renderer) const {
    renderer.drawAlbum(album, newPhotoIndex);
}

void AlbumScene::enter() {
    album.addPhoto(stage);
    newPhotoIndex = album.getPhotoCount() - 1;
    finished = false;
}

void AlbumScene::update(int /*elapsedMs*/, const std::vector<Action>& actions) {
    if (std::find(actions.begin(), actions.end(), Action::Confirm) != actions.end()) {
        finished = true;
    }
}
