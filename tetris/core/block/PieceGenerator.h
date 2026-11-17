#pragma once

#include "block/Tetromino.h"

// 다음에 나올 블록을 정하는 생성기의 추상 클래스.
// 블록을 쓰는 쪽(PlayField)은 이 클래스에만 의존하므로,
// 블록을 고르는 방식이 바뀌어도 쓰는 쪽은 고치지 않는다. (DIP)
//
// 사용 방법: peekNext()로 다음 블록을 확인해 clone()으로 복사한 뒤, advance()로 넘긴다.
class PieceGenerator {
public:
    virtual ~PieceGenerator();

    // ===== 쿼리 =====

    // 다음에 나올 블록. 여러 번 불러도 advance() 전까지는 같은 블록이다.
    virtual const Tetromino& peekNext() const = 0;

    // ===== 커맨드 =====

    // 다음 블록을 하나 넘긴다.
    virtual void advance() = 0;
};
