#pragma once

#include <memory>
#include <vector>

#include "common/Point.h"

// 테트로미노(4칸 블록)의 추상 클래스.
// 블록 종류마다 다른 것(모양, 회전 상자 크기, 기호)만 자식 클래스가 정하고,
// 이동·회전·칸 계산은 이 클래스가 모두 처리한다.
// 새 블록 종류는 이 클래스를 상속한 클래스를 추가하기만 하면 된다. (OCP)
class Tetromino {
public:
    virtual ~Tetromino() = default;

    // ===== 쿼리 =====

    // 현재 위치와 회전 상태에서 블록이 차지하는 보드 칸 4개
    std::vector<Point> getCells() const;

    // (deltaRow, deltaCol)만큼 옮겼다면 차지할 보드 칸. 블록은 실제로 움직이지 않는다.
    std::vector<Point> getCellsAfterMove(int deltaRow, int deltaCol) const;

    // 시계 방향으로 한 번 돌렸다면 차지할 보드 칸. 블록은 실제로 회전하지 않는다.
    std::vector<Point> getCellsAfterRotate() const;

    // 회전 상자의 왼쪽 위 칸이 놓인 보드 위치
    Point getPosition() const;

    // 회전 상태 (0 ~ 3, 시계 방향으로 한 번 돌 때마다 1씩 증가)
    int getRotation() const;

    // 회전 상자 한 변의 길이. 블록을 보드 가운데에 놓을 때 사용한다.
    virtual int getBoxSize() const = 0;

    // 블록 종류를 구분하는 기호 ('I', 'O' 등). 화면 담당 클래스가 색이나 모양을 정할 때 사용한다.
    virtual char getSymbol() const = 0;

    // 같은 종류·위치·회전 상태를 가진 새 블록을 만든다.
    // 구체 클래스를 몰라도 블록을 복사할 수 있게 해 준다. (DIP)
    virtual std::unique_ptr<Tetromino> clone() const = 0;

    // ===== 커맨드 =====

    void moveBy(int deltaRow, int deltaCol);
    void moveTo(const Point& newPosition);
    void rotateClockwise();

protected:
    Tetromino();

    // 회전하지 않은 상태의 모양. 회전 상자 왼쪽 위를 (0, 0)으로 하는 칸 4개다.
    virtual std::vector<Point> getBaseShape() const = 0;

private:
    // rotationCount번 시계 방향으로 돌린 모양 (회전 상자 기준)
    std::vector<Point> getShape(int rotationCount) const;

    // 회전 상자 기준 모양을 보드 칸으로 바꾼다
    std::vector<Point> toBoardCells(const std::vector<Point>& shape, const Point& origin) const;

    Point position;
    int rotation;
};
