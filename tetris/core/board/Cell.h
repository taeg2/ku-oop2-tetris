#pragma once
#include <optional>
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
    std::optional<PillType> pill() const { return m_pill; }

private:
    bool m_filled = false;
    Color m_color = Color::Black;
    std::optional<PillType> m_pill;
};
