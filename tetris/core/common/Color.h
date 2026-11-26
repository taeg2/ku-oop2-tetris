#pragma once
#include <cstdint>
using namespace std;

// 콘솔 16색 팔레트. 번호는 Windows 콘솔 색 속성값과 같다.
enum class Color : uint8_t {
    Black, DarkBlue, DarkGreen, DarkCyan, DarkRed, DarkMagenta, DarkYellow, Gray,
    DarkGray, Blue, Green, Cyan, Red, Magenta, Yellow, White
};
