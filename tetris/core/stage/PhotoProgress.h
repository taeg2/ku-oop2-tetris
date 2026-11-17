#pragma once
#include "play/EventBus.h"

// 사진 4분할 진행도. 지운 줄을 누적해 3줄마다 조각 하나를 채우고 이벤트를 알린다.
//     속도·보스·HUD는 이 클래스가 직접 부르지 않고 이벤트로만 반응한다.
class PhotoProgress {
public:
    explicit PhotoProgress(EventBus& bus);

    void addLines(int count);
    int pieces() const { return m_pieces; }
    int linesUntilNextPiece() const;
    bool isHalf() const;
    bool isComplete() const;
    void reset();

private:
    EventBus& m_bus;
    int m_lines = 0;
    int m_pieces = 0;
};
