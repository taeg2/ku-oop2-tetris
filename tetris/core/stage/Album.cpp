#include "stage/Album.h"

Album::Album() : photos() {}

int Album::getPhotoCount() const {
    return static_cast<int>(photos.size());
}

const Stage& Album::getPhoto(int index) const {
    return *photos.at(index);
}

void Album::addPhoto(const Stage& stage) {
    photos.push_back(&stage);
}

void Album::clear() {
    photos.clear();
}
