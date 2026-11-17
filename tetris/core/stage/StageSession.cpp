#include "stage/StageSession.h"

#include <utility>

StageSession::StageSession(Stage& stage,
                           PlayField& field,
                           MedicineInventory& medicines,
                           int linesPerPiece,
                           std::vector<std::unique_ptr<Medicine>> rewardPrototypes)
    : stage(stage),
      field(field),
      medicines(medicines),
      medicinesAtStart(medicines),
      progress(linesPerPiece),
      rewardPrototypes(std::move(rewardPrototypes)),
      nextRewardIndex(0),
      cleared(false) {}

const Stage& StageSession::getStage() const {
    return stage;
}

const PhotoProgress& StageSession::getProgress() const {
    return progress;
}

const MedicineInventory& StageSession::getMedicines() const {
    return medicines;
}

bool StageSession::isCleared() const {
    return cleared;
}

bool StageSession::isFailed() const {
    return !cleared && field.isGameOver();
}

void StageSession::start() {
    medicinesAtStart.restoreFrom(medicines);
    field.setListener(this);
    restart();
}

void StageSession::restart() {
    field.reset();
    progress.reset();
    stage.disappearBoss();
    medicines.restoreFrom(medicinesAtStart);
    nextRewardIndex = 0;
    cleared = false;
}

void StageSession::update(int elapsedMs) {
    if (!isPlaying()) {
        return;
    }
    field.update(elapsedMs);
    if (isPlaying()) {
        stage.updateBoss(elapsedMs, field);
    }
}

void StageSession::useMedicine(int index) {
    if (!isPlaying()) {
        return;
    }
    medicines.use(index, field);
}

void StageSession::onPieceLocked() {
    // 블록이 고정될 때 하는 일은 아직 없다. (턴 단위 기믹을 추가할 때 사용)
}

void StageSession::onLinesCleared(int count) {
    if (cleared) {
        return;
    }

    const int filledBefore = progress.getFilledPieceCount();
    const bool wasHalfFilled = progress.isHalfFilled();
    progress.addClearedLines(count);

    for (int i = filledBefore; i < progress.getFilledPieceCount(); ++i) {
        onPhotoPieceFilled();
    }

    if (progress.isComplete()) {
        stage.disappearBoss();
        cleared = true;
    }
    else if (!wasHalfFilled && progress.isHalfFilled()) {
        stage.appearBoss();
    }
}

void StageSession::onGameOver() {
    stage.disappearBoss();
}

bool StageSession::isPlaying() const {
    return !cleared && !field.isGameOver();
}

void StageSession::onPhotoPieceFilled() {
    field.speedUp();

    if (rewardPrototypes.empty() || medicines.isFull()) {
        return;
    }
    medicines.add(rewardPrototypes[nextRewardIndex]->clone());
    nextRewardIndex = (nextRewardIndex + 1) % static_cast<int>(rewardPrototypes.size());
}
