#pragma once

#include <memory>
#include <vector>

#include "block/PieceGenerator.h"
#include "block/Tetromino.h"
#include "board/Board.h"
#include "common/Point.h"
#include "play/EffectTarget.h"
#include "play/FallSpeed.h"
#include "play/PlayListener.h"

// 테트리스 한 판의 진행을 담당한다.
// 떨어지는 블록의 조작, 시간에 따른 낙하, 고정, 줄 삭제, 다음 블록 생성, 게임 오버를 처리한다.
//
// - 블록은 PieceGenerator(추상 클래스)에서 받으므로 구체 블록 클래스를 모른다. (DIP)
// - 일어난 일은 PlayListener(추상 클래스)에 알리므로 스테이지 규칙을 모른다. (DIP)
// - 보스와 약은 EffectTarget(추상 클래스)으로만 이 클래스를 다룬다.
// - 화면 출력과 키 입력은 하지 않는다. 시간도 update(elapsedMs)로 전달받는다.
class PlayField : public EffectTarget {
public:
    PlayField(int width, int height, PieceGenerator& generator, const FallSpeed& speed = FallSpeed());

    // ===== 쿼리 =====

    const Board& getBoard() const override;

    // 지금 떨어지고 있는 블록. 게임 오버 상태에서는 놓지 못한 블록이다.
    const Tetromino& getCurrentPiece() const;

    // 다음에 나올 블록 (미리보기)
    const Tetromino& getNextPiece() const;

    const FallSpeed& getSpeed() const;

    bool isGameOver() const;

    // ===== 커맨드: 설정 =====

    // 일어난 일을 알릴 대상을 정한다. nullptr이면 알리지 않는다.
    void setListener(PlayListener* newListener);

    // ===== 커맨드: 플레이어 조작 (게임 오버 상태에서는 아무 일도 하지 않는다) =====

    void moveLeft();
    void moveRight();

    // 시계 방향으로 회전한다. 벽이나 블록에 막히면 옆으로 조금 밀어서 다시 시도한다.
    void rotate();

    // 한 칸 내린다. 더 내려갈 수 없으면 그 자리에 고정한다.
    void softDrop();

    // 내려갈 수 있는 곳까지 바로 떨어뜨리고 고정한다.
    void hardDrop();

    // ===== 커맨드: 진행 =====

    // 시간이 elapsedMs만큼 흘렀음을 알린다. 낙하 간격마다 블록이 한 칸씩 떨어진다.
    void update(int elapsedMs);

    // 낙하 속도를 한 단계 올린다. (사진 조각을 채울 때)
    void speedUp();

    // 보드와 속도를 처음 상태로 되돌리고 새 블록을 꺼낸다. (스테이지 시작·재도전)
    void reset();

    // ===== 커맨드: 보스·약 효과 (EffectTarget) =====

    void removeCells(const std::vector<Point>& cells) override;
    void removeBottomLine() override;
    void transformCurrentPiece() override;
    void slowDown(int durationMs) override;

private:
    // 다음 블록을 꺼내 맨 위 가운데에 놓는다. 놓을 수 없으면 게임 오버가 된다.
    void spawnNextPiece();

    // 블록을 (deltaRow, deltaCol)만큼 옮길 수 있으면 옮긴다.
    void tryMove(int deltaRow, int deltaCol);

    // 한 칸 내리거나, 내려갈 수 없으면 고정한다.
    void stepDown();

    // 블록을 보드에 고정하고, 꽉 찬 줄을 지우고, 다음 블록을 꺼낸 뒤 알린다.
    void lockCurrentPiece();

    // 보드가 바뀌어 떨어지던 블록과 겹치면, 겹치지 않을 때까지 블록을 위로 올린다.
    void pushCurrentPieceUpIfBlocked();

    bool canMoveDown() const;

    Board board;
    std::unique_ptr<Tetromino> currentPiece;
    PieceGenerator& generator;
    FallSpeed speed;
    PlayListener* listener;
    int fallTimerMs;
    bool gameOver;
};
