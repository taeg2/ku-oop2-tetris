#include "play/PlayField.h"

#include <utility>

namespace {
    // 회전하거나 모양이 바뀐 블록이 막혔을 때 옆으로 밀어 보는 칸 수 (차례대로 시도한다)
    const int kickOffsets[] = { 0, -1, 1, -2, 2 };

    // 모양을 바꿀 때, 지금과 다른 모양이 나올 때까지 생성기를 넘겨 보는 최대 횟수
    const int maxTransformAttempts = 7;

    std::vector<Point> shiftCells(const std::vector<Point>& cells, int deltaCol) {
        std::vector<Point> shifted;
        for (const auto& cell : cells) {
            shifted.push_back(cell.translated(0, deltaCol));
        }
        return shifted;
    }
}

PlayField::PlayField(int width, int height, PieceGenerator& generator, const FallSpeed& speed)
    : board(width, height),
      currentPiece(),
      generator(generator),
      speed(speed),
      listener(nullptr),
      fallTimerMs(0),
      gameOver(false) {
    spawnNextPiece();
}

const Board& PlayField::getBoard() const {
    return board;
}

const Tetromino& PlayField::getCurrentPiece() const {
    return *currentPiece;
}

const Tetromino& PlayField::getNextPiece() const {
    return generator.peekNext();
}

const FallSpeed& PlayField::getSpeed() const {
    return speed;
}

bool PlayField::isGameOver() const {
    return gameOver;
}

void PlayField::setListener(PlayListener* newListener) {
    listener = newListener;
}

void PlayField::moveLeft() {
    tryMove(0, -1);
}

void PlayField::moveRight() {
    tryMove(0, 1);
}

void PlayField::rotate() {
    if (gameOver) {
        return;
    }
    const std::vector<Point> rotated = currentPiece->getCellsAfterRotate();
    for (int offset : kickOffsets) {
        if (board.canPlace(shiftCells(rotated, offset))) {
            currentPiece->rotateClockwise();
            currentPiece->moveBy(0, offset);
            return;
        }
    }
}

void PlayField::softDrop() {
    if (gameOver) {
        return;
    }
    fallTimerMs = 0;
    stepDown();
}

void PlayField::hardDrop() {
    if (gameOver) {
        return;
    }
    while (canMoveDown()) {
        currentPiece->moveBy(1, 0);
    }
    lockCurrentPiece();
}

void PlayField::update(int elapsedMs) {
    if (gameOver) {
        return;
    }
    speed.update(elapsedMs);
    fallTimerMs += elapsedMs;
    while (!gameOver && fallTimerMs >= speed.getFallIntervalMs()) {
        fallTimerMs -= speed.getFallIntervalMs();
        stepDown();
    }
}

void PlayField::speedUp() {
    speed.speedUp();
}

void PlayField::reset() {
    board.reset();
    speed.reset();
    fallTimerMs = 0;
    gameOver = false;
    spawnNextPiece();
}

void PlayField::removeCells(const std::vector<Point>& cells) {
    board.clearCells(cells);
}

void PlayField::removeBottomLine() {
    board.clearBottomLine();
    pushCurrentPieceUpIfBlocked();
}

void PlayField::transformCurrentPiece() {
    if (gameOver) {
        return;
    }

    // 지금과 다른 모양이 나올 때까지 생성기를 넘긴다
    for (int i = 0; i < maxTransformAttempts; ++i) {
        if (generator.peekNext().getSymbol() != currentPiece->getSymbol()) {
            break;
        }
        generator.advance();
    }
    if (generator.peekNext().getSymbol() == currentPiece->getSymbol()) {
        return;
    }

    // 같은 위치·회전 상태로 새 블록을 만들고, 막히면 옆으로 밀어 본다
    std::unique_ptr<Tetromino> replacement = generator.peekNext().clone();
    replacement->moveTo(currentPiece->getPosition());
    for (int i = 0; i < currentPiece->getRotation(); ++i) {
        replacement->rotateClockwise();
    }
    for (int offset : kickOffsets) {
        if (board.canPlace(replacement->getCellsAfterMove(0, offset))) {
            replacement->moveBy(0, offset);
            currentPiece = std::move(replacement);
            generator.advance();
            return;
        }
    }
}

void PlayField::slowDown(int durationMs) {
    speed.slowDown(durationMs);
}

void PlayField::spawnNextPiece() {
    currentPiece = generator.peekNext().clone();
    generator.advance();

    const int spawnCol = (board.getWidth() - currentPiece->getBoxSize()) / 2;
    currentPiece->moveTo(Point(0, spawnCol));
    if (!board.canPlace(currentPiece->getCells())) {
        gameOver = true;
    }
}

void PlayField::tryMove(int deltaRow, int deltaCol) {
    if (gameOver) {
        return;
    }
    if (board.canPlace(currentPiece->getCellsAfterMove(deltaRow, deltaCol))) {
        currentPiece->moveBy(deltaRow, deltaCol);
    }
}

void PlayField::stepDown() {
    if (canMoveDown()) {
        currentPiece->moveBy(1, 0);
    }
    else {
        lockCurrentPiece();
    }
}

void PlayField::lockCurrentPiece() {
    board.lockCells(currentPiece->getCells(), currentPiece->getSymbol());

    const std::vector<int> fullLines = board.getFullLines();
    board.clearLines(fullLines);

    fallTimerMs = 0;
    spawnNextPiece();

    // 상태를 모두 바꾼 뒤에 알린다. 알림을 받는 쪽이 reset() 등을 불러도 안전하다.
    if (listener != nullptr) {
        listener->onPieceLocked();
    }
    if (listener != nullptr && !fullLines.empty()) {
        listener->onLinesCleared(static_cast<int>(fullLines.size()));
    }
    if (listener != nullptr && gameOver) {
        listener->onGameOver();
    }
}

void PlayField::pushCurrentPieceUpIfBlocked() {
    for (int i = 0; i < board.getHeight(); ++i) {
        if (board.canPlace(currentPiece->getCells())) {
            return;
        }
        currentPiece->moveBy(-1, 0);
    }
}

bool PlayField::canMoveDown() const {
    return board.canPlace(currentPiece->getCellsAfterMove(1, 0));
}
