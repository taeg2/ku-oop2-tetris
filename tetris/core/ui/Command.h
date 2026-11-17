#pragma once

// 키 입력을 게임 의미로 바꾼 명령. 로직은 키 코드를 모르고 이 값만 받는다.
enum class Command {
    MoveLeft, MoveRight, Rotate, SoftDrop, HardDrop,
    UseBluePill, UseRedPill,
    Confirm, Pause, Quit
};
