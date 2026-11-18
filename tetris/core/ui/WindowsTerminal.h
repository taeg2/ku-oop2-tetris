#pragma once
#include "ui/ITerminal.h"

// Windows 콘솔 API로 출력한다. windows.h는 WindowsTerminal.cpp에서만 include한다.
// Windows가 아닌 환경에서는 이 클래스를 빌드에서 사용하지 않는다 (main.cpp 참고).
class WindowsTerminal : public ITerminal {
public:
    void open(int width, int height) override;
    void close() override;
    void clearScreen() override;
    void write(Point pos, const std::u32string& text, Color fg, Color bg) override;
    void flush() override;

private:
    void setCursorVisible(bool visible);

    void* m_output = nullptr;  // HANDLE
    void* m_input = nullptr;   // HANDLE
    unsigned long m_originalInputMode = 0;
};
