#pragma once
#include <vector>
#include "stage/PhotoArt.h"
using namespace std;

// 지금까지 모은 가족사진. 스테이지를 깰 때마다 한 장씩 늘어난다.
class Album {
public:
    explicit Album(int capacity);
    void add(const PhotoArt& photo);
    const vector<PhotoArt>& photos() const { return m_photos; }
    int capacity() const { return m_capacity; }
    bool isComplete() const;
    void clear();  // 새 게임

private:
    int m_capacity;
    vector<PhotoArt> m_photos;
};
