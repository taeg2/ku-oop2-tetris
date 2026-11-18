#pragma once

// 정수 좌표 한 점. 보드 칸 좌표, 콘솔 좌표, 사진 픽셀 좌표에 함께 쓴다.
struct Point {
    int x = 0;
    int y = 0;

    constexpr Point operator+(Point rhs) const { return {x + rhs.x, y + rhs.y}; }
    constexpr Point operator-(Point rhs) const { return {x - rhs.x, y - rhs.y}; }
    Point& operator+=(Point rhs) { x += rhs.x; y += rhs.y; return *this; }
    constexpr bool operator==(Point rhs) const { return x == rhs.x && y == rhs.y; }
    constexpr bool operator!=(Point rhs) const { return !(*this == rhs); }
};
