#pragma once
#include <memory>
#include "ui/KeyboardInput.h"
using namespace std;

// Mac/Linux 터미널 키보드 입력. 터미널을 "한 글자씩, 기다리지 않고" 읽는 모드로 바꾸고,
// 끝나면 원래 모드로 되돌린다. termios는 .cpp에서만 include한다.
class PosixKeyboardInput : public KeyboardInput {
public:
    PosixKeyboardInput();
    ~PosixKeyboardInput() override;
    PosixKeyboardInput(const PosixKeyboardInput&) = delete;
    PosixKeyboardInput& operator=(const PosixKeyboardInput&) = delete;

    void update() override;

private:
    class SavedMode;  // termios 설정 (헤더에서 숨김)
    unique_ptr<SavedMode> m_saved;
};
