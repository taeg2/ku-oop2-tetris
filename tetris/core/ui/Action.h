#pragma once

// 플레이어가 할 수 있는 조작.
// 입력 담당 클래스(InputSource)가 키 입력을 이 값으로 바꿔서 전달한다.
// 게임 쪽은 어떤 키를 눌렀는지 모르고 조작만 알기 때문에, 키 배치를 바꿔도 게임 코드는 고치지 않는다.
//
// 열거형이라 구현할 내용이 없어서 .cpp 파일은 두지 않는다.
enum class Action {
    MoveLeft,
    MoveRight,
    SoftDrop,
    HardDrop,
    Rotate,
    UseMedicine1,
    UseMedicine2,
    Confirm,
    Quit
};
