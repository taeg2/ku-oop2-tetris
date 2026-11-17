#pragma once

// PlayField에서 일어난 일을 전달받는 쪽의 추상 클래스.
// PlayField는 누가 듣는지 모른 채 알리기만 하므로,
// 사진 조각·보스 같은 스테이지 규칙이 바뀌어도 PlayField는 고치지 않는다. (DIP)
//
// 알림은 PlayField의 상태가 모두 바뀐 뒤에 온다. 그래서 알림을 받는 쪽에서
// PlayField의 커맨드(reset 등)를 불러도 된다.
class PlayListener {
public:
    virtual ~PlayListener();

    // 블록이 바닥이나 다른 블록에 닿아 고정되었다.
    virtual void onPieceLocked() = 0;

    // 블록이 고정되면서 count줄이 지워졌다. (1 ~ 4)
    virtual void onLinesCleared(int count) = 0;

    // 새 블록을 놓을 공간이 없어 게임이 끝났다.
    virtual void onGameOver() = 0;
};
