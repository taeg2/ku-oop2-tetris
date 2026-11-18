#pragma once
#include "common/Point.h"
#include "ui/Console.h"

// 화면 구성 요소의 추상 클래스. 장면은 View 목록을 들고 update()/draw()를 한꺼번에 부른다.
//     View는 게임 상태를 const로 읽기만 하고 바꾸지 않는다.
class View {
public:
    explicit View(Point origin);
    virtual ~View() = default;

    virtual void update(int dtMs);
    virtual void draw(Console& console) const = 0;

    void setOrigin(Point origin);
    Point origin() const;

protected:
    bool blink(int periodMs) const;  // periodMs마다 켜짐/꺼짐이 바뀐다

    Point m_origin;
    int m_timeMs = 0;
};
