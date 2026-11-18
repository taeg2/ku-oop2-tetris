#include "stage/Album.h"
#include "common/GameException.h"

Album::Album(int capacity) : m_capacity(capacity) {}

void Album::add(const PhotoArt& photo) {
    if (isComplete()) {
        throw GameException("앨범이 이미 가득 찼습니다");
    }
    m_photos.push_back(photo);
}

bool Album::isComplete() const {
    return static_cast<int>(m_photos.size()) >= m_capacity;
}

void Album::clear() {
    m_photos.clear();
}
