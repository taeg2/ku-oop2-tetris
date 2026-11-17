#pragma once

// 보드 위의 한 칸 위치를 나타낸다. 행(row)은 아래로, 열(col)은 오른쪽으로 갈수록 커진다.
class Point {
public:
    Point();
    Point(int row, int col);

    int getRow() const;
    int getCol() const;

    // 이 위치에서 (deltaRow, deltaCol)만큼 옮긴 새 위치를 돌려준다. 자기 자신은 바뀌지 않는다.
    Point translated(int deltaRow, int deltaCol) const;

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

private:
    int row;
    int col;
};
