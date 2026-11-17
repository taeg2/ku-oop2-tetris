#include "stage/PhotoProgress.h"

#include <algorithm>
#include <stdexcept>

PhotoProgress::PhotoProgress(int linesPerPiece)
    : linesPerPiece(linesPerPiece), clearedLines(0) {
    if (linesPerPiece < 1) {
        throw std::invalid_argument("PhotoProgress: 조각당 줄 수는 1 이상이어야 한다");
    }
}

int PhotoProgress::getFilledPieceCount() const {
    return std::min(pieceCount, clearedLines / linesPerPiece);
}

int PhotoProgress::getPieceCount() const {
    return pieceCount;
}

int PhotoProgress::getClearedLines() const {
    return clearedLines;
}

int PhotoProgress::getLinesUntilNextPiece() const {
    if (isComplete()) {
        return 0;
    }
    return linesPerPiece - clearedLines % linesPerPiece;
}

bool PhotoProgress::isHalfFilled() const {
    return getFilledPieceCount() >= pieceCount / 2;
}

bool PhotoProgress::isComplete() const {
    return getFilledPieceCount() == pieceCount;
}

void PhotoProgress::addClearedLines(int count) {
    if (count > 0) {
        clearedLines += count;
    }
}

void PhotoProgress::reset() {
    clearedLines = 0;
}
