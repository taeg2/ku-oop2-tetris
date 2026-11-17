#include "stage/Stage.h"

#include <stdexcept>
#include <utility>

Stage::Stage(int number,
             const std::string& title,
             const std::string& blockTheme,
             const std::array<std::string, 4>& photoParts,
             const std::string& albumCaption,
             std::unique_ptr<Boss> boss)
    : number(number),
      title(title),
      blockTheme(blockTheme),
      photoParts(photoParts),
      albumCaption(albumCaption),
      boss(std::move(boss)) {
    if (this->boss == nullptr) {
        throw std::invalid_argument("Stage: 보스가 필요하다");
    }
}

int Stage::getNumber() const {
    return number;
}

std::string Stage::getTitle() const {
    return title;
}

int Stage::getPhotoPartCount() const {
    return photoPartCount;
}

std::string Stage::getBlockTheme() const {
    return blockTheme;
}

std::string Stage::getPhotoPart(int index) const {
    if (index < 0 || index >= photoPartCount) {
        return "";
    }
    return photoParts[index];
}

std::string Stage::getAlbumCaption() const {
    return albumCaption;
}

const Boss& Stage::getBoss() const {
    return *boss;
}

void Stage::appearBoss() {
    boss->appear();
}

void Stage::disappearBoss() {
    boss->disappear();
}

void Stage::updateBoss(int elapsedMs, EffectTarget& target) {
    boss->update(elapsedMs, target);
}
