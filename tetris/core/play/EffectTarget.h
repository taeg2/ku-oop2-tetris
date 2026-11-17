#pragma once

#include <vector>

#include "board/Board.h"
#include "common/Point.h"

// 보스 기믹과 약 효과가 적용되는 대상의 추상 클래스.
// 보스와 약은 이 클래스에만 의존하므로, 새 보스·약은 이 함수들만으로 효과를 만들 수 있다. (DIP)
// PlayField가 이 클래스를 구현한다.
class EffectTarget {
public:
    virtual ~EffectTarget();

    // ===== 쿼리 =====

    // 현재 보드. 지울 칸을 고를 때 쓴다.
    virtual const Board& getBoard() const = 0;

    // ===== 커맨드 =====

    // 보드에 쌓인 칸 중 주어진 칸을 비운다. (기억을 잊는 보스)
    virtual void removeCells(const std::vector<Point>& cells) = 0;

    // 보드 맨 아래 한 줄을 없앤다. (빨간 약)
    virtual void removeBottomLine() = 0;

    // 떨어지고 있는 블록을 다른 모양으로 바꾼다. (모양을 바꾸는 보스)
    virtual void transformCurrentPiece() = 0;

    // durationMs 동안 낙하 속도를 늦춘다. (파란 약)
    virtual void slowDown(int durationMs) = 0;
};
