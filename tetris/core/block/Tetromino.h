#pragma once
#include <array>
#include "common/Color.h"
#include "medicine/PillType.h"
#include "common/Point.h"
#include "block/TetrominoType.h"
using namespace std;

// 떨어지는 블록 하나. 값 타입이라 이동, 회전, 변신은 모두 "새 블록을 돌려주는" 방식이다.
//     그래서 충돌 검사 전에 원본을 망가뜨리지 않는다.
class Tetromino {
public:
    inline static const int kCellCount = 4;
    using Cells = array<Point, kCellCount>;

    explicit Tetromino(TetrominoType type = TetrominoType::T);
    Tetromino(TetrominoType type, Point position);

    Cells cells() const;       // 보드 좌표
    Cells localCells() const;  // 회전만 반영된 블록 내부 좌표 (미리보기용)

    Tetromino moved(Point delta) const;
    Tetromino rotated() const;
    Tetromino rotated(int times) const;
    Tetromino morphed(TetrominoType type) const;
    Tetromino withPill(PillType pill, int cellIndex) const;

    TetrominoType type() const { return m_type; }
    Point position() const { return m_position; }
    int rotation() const { return m_rotation; }
    Color color() const { return colorOf(m_type); }
    bool hasPill() const { return m_hasPill; }
    PillType pill() const { return m_pill; }  // hasPill()이 true일 때만 의미가 있다
    int pillCellIndex() const { return m_pillIndex; }

    static Color colorOf(TetrominoType type);

private:
    TetrominoType m_type;
    int m_rotation = 0;
    Point m_position{};
    bool m_hasPill = false;
    PillType m_pill = PillType::Blue;
    int m_pillIndex = 0;
};
