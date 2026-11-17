#pragma once

#include <vector>

#include "common/Point.h"

// 블록이 쌓이는 테트리스 보드.
// 칸마다 쌓인 블록의 기호(Tetromino::getSymbol())를 저장한다. 빈 칸인지는 isEmpty()로 확인한다.
// 보드는 블록 종류를 모르고 기호만 저장하므로, 새 블록 종류가 생겨도 고치지 않는다.
class Board {
public:
    Board(int width, int height);

    // ===== 쿼리 =====

    int getWidth() const;
    int getHeight() const;

    // 보드 범위 안의 칸인지
    bool isInside(const Point& cell) const;

    // 보드 안의 빈 칸인지. 보드 밖이면 false다.
    bool isEmpty(const Point& cell) const;

    // 칸에 쌓인 블록의 기호. 빈 칸이거나 보드 밖이면 공백(' ')이다.
    char getSymbolAt(const Point& cell) const;

    // 주어진 칸이 모두 보드 안의 빈 칸이어서 블록을 놓을 수 있는지
    bool canPlace(const std::vector<Point>& cells) const;

    // 가로 한 줄이 꽉 찬 행 번호들 (위에서 아래 순서)
    std::vector<int> getFullLines() const;

    // 블록이 쌓여 있는 모든 칸
    std::vector<Point> getFilledCells() const;

    // ===== 커맨드 =====

    // 주어진 칸에 블록 기호를 쌓는다. 보드 밖의 칸은 무시한다.
    void lockCells(const std::vector<Point>& cells, char symbol);

    // 주어진 행들을 지우고, 그 위의 줄들을 지운 줄 수만큼 아래로 내린다.
    void clearLines(const std::vector<int>& rows);

    // 맨 아래 한 줄을 지운다. (빨간 약)
    void clearBottomLine();

    // 주어진 칸만 비운다. 위의 블록은 내려오지 않는다. (보스 기믹)
    void clearCells(const std::vector<Point>& cells);

    // 보드를 모두 비운다.
    void reset();

private:
    static constexpr char emptySymbol = ' ';

    bool isLineFull(int row) const;
    std::vector<char> makeEmptyLine() const;

    int width;
    int height;
    std::vector<std::vector<char>> cells;
};
