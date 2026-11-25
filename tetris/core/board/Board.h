#pragma once
#include <array>
#include <vector>
#include "board/Cell.h"
#include "medicine/PillType.h"
#include "common/Point.h"
#include "common/Rect.h"
#include "block/Tetromino.h"

// 테트리스 보드 (10 x 20). 내부 격자는 감추고, 블록 놓기·줄 지우기·칸 조작을 메서드로만 허용한다.
//     보스 기믹과 약도 반드시 이 메서드를 통해서만 보드를 바꾼다.
//     가림(hidden)·표시(marked)는 보드 위에 덮는 막이라, 줄이 내려가도 같이 움직이지 않는다.
class Board {
public:
    inline static const int kWidth = 10;
    inline static const int kHeight = 20;

    Board();

    bool canPlace(const Tetromino& piece) const;
    bool canPlace(const Tetromino& piece, Point offset) const;
    bool canLock(const Tetromino& piece) const;  // 블록이 보드 위쪽 밖에 걸치면 false (실패 조건)
    void lock(const Tetromino& piece);

    std::vector<int> findFullLines() const;
    std::vector<PillType> pillsInRows(const std::vector<int>& rows) const;
    void clearRows(const std::vector<int>& rows);
    void clearRow(int y);
    void clearRow();  // 맨 아래 줄 (빨간 약)
    bool isRowEmpty(int y) const;

    void removeCell(Point pos);
    void removeCells(const std::vector<Point>& cells);

    void setHidden(const Rect& area, bool hidden);
    void setHidden(bool hidden);  // 보드 전체
    bool isHidden(Point pos) const;

    void setMarked(const std::vector<Point>& cells, bool marked);
    void setMarked(bool marked);  // 보드 전체
    bool isMarked(Point pos) const;

    std::vector<Point> filledCells() const;
    const Cell& cellAt(Point pos) const;  // 범위 밖이면 std::out_of_range
    bool isInside(Point pos) const;
    void reset();

private:
    template <typename T>
    using Grid = std::array<std::array<T, kWidth>, kHeight>;

    Cell& at(Point pos);

    Grid<Cell> m_grid{};
    Grid<bool> m_hidden{};
    Grid<bool> m_marked{};
};
