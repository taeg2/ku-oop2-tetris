#pragma once

#include <vector>

#include "stage/Stage.h"

// 완성한 가족사진을 모아 두는 앨범.
// 스테이지를 클리어할 때마다 그 스테이지의 사진이 순서대로 한 장씩 들어간다.
// 사진 내용은 Stage가 가지고 있으므로, 앨범은 Stage를 가리키기만 한다.
class Album {
public:
    Album();

    // ===== 쿼리 =====

    int getPhotoCount() const;

    // index번째(0부터)로 들어온 사진의 스테이지. 범위를 벗어나면 std::out_of_range 예외가 난다.
    const Stage& getPhoto(int index) const;

    // ===== 커맨드 =====

    void addPhoto(const Stage& stage);

    // 앨범을 비운다. (게임을 처음부터 다시 할 때)
    void clear();

private:
    std::vector<const Stage*> photos;
};
