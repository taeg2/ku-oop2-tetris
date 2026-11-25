#pragma once
#include "common/Color.h"
#include "medicine/PillType.h"

// 보드의 한 칸. 채워졌는지, 무슨 색인지, 약이 붙어 있는지만 안다.
class Cell {
public:
    void fill(Color color);
    void fill(Color color, PillType pill);
    void clear();

    bool isFilled() const { return m_filled; }
    Color color() const { return m_color; }
    bool hasPill() const { return m_hasPill; }
    PillType pill() const { return m_pill; }  // hasPill()이 true일 때만 의미가 있다

private:
    bool m_filled = false;
    Color m_color = Color::Black;
    bool m_hasPill = false;
    PillType m_pill = PillType::Blue;
};
