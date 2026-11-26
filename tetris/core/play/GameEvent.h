#pragma once

// 스테이지 안에서 일어난 일. EventBus로 퍼져서 관심 있는 객체만 반응한다.
class GameEvent {
public:
    enum class Type {
        PieceLocked,    // 블록이 바닥에 고정됨
        LinesCleared,   // value = 지운 줄 수, row = 맨 위 줄
        PieceFilled,    // value = 채워진 사진 조각 수 (1~4)
        PhotoHalf,      // 사진 2/4 -> 보스 등장
        PhotoComplete,  // 사진 4/4 -> 보스 퇴장, 스테이지 클리어
        BossTriggered,  // 보스 기믹 발동
        PillGained,     // value = PillType
        PillUsed,       // value = PillType
        StageFailed
    };

    Type type;
    int value = 0;
    int row = -1;
};
