#pragma once
#include "ui/KeyboardInput.h"

// Windows 콘솔 키보드 입력 (_kbhit / _getch). conio.h는 .cpp에서만 include한다.
class WindowsKeyboardInput : public KeyboardInput {
public:
    void update() override;
};
