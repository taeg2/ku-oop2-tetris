#pragma once
#include <optional>
#include "block/Tetromino.h"
#include "block/TetrominoType.h"
#include "common/Color.h"
#include "medicine/PillType.h"

// 보드의 한 칸. 어떤 블록으로 채워졌는지, 약이 붙어 있는지만 안다.
//     색은 따로 저장하지 않고 블록 종류에서 계산한다. (정보 전문가)
class Cell {
public:
    void fill(TetrominoType type);
    void fill(TetrominoType type, PillType pill);
    void clear();

    bool isFilled() const { return m_type.has_value(); }
    std::optional<TetrominoType> type() const { return m_type; }
    Color color() const { return m_type ? Tetromino::colorOf(*m_type) : Color::Black; }
    std::optional<PillType> pill() const { return m_pill; }

private:
    std::optional<TetrominoType> m_type;  // 비어 있으면 nullopt
    std::optional<PillType> m_pill;
};