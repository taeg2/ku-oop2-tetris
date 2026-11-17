#pragma once

#include <string>

#include "common/Point.h"
#include "play/EffectTarget.h"

// 보스의 추상 클래스.
// 등장·퇴장과 "일정 주기마다 기믹 발동"은 이 클래스가 처리하고,
// 기믹이 실제로 하는 일(activateGimmick)만 자식 클래스가 정한다.
// 새 보스는 이 클래스를 상속해 activateGimmick만 구현하면 된다. (OCP)
//
// 보스는 EffectTarget(추상 클래스)으로만 게임에 효과를 건다. (DIP)
class Boss {
public:
    virtual ~Boss();

    // ===== 쿼리 =====

    // 보스 이름 (기획안에서 아직 정해지지 않아 main에서 넘겨준다)
    std::string getName() const;

    // 등장해 있는지 (사진 2/4 ~ 4/4 사이)
    bool isActive() const;

    // 다음 기믹이 발동하기까지 남은 시간. 등장해 있지 않으면 0이다.
    int getMsUntilNextGimmick() const;

    // 화면에서 이 칸을 가려야 하는지. 기본은 가리지 않는다. (암전 보스가 오버라이드한다)
    virtual bool hidesCell(const Point& cell) const;

    // ===== 커맨드 =====

    // 등장한다. 기믹 타이머와 기믹 상태를 처음부터 시작한다.
    void appear();

    // 퇴장한다. 남아 있던 기믹 효과도 없앤다.
    void disappear();

    // 시간이 elapsedMs만큼 흘렀음을 알린다. 등장해 있으면 주기마다 기믹을 발동한다.
    void update(int elapsedMs, EffectTarget& target);

protected:
    // periodMs: 기믹이 발동하는 주기 (기획안 미정이라 main에서 넘겨준다)
    Boss(const std::string& name, int periodMs);

    // 기믹을 한 번 발동한다.
    virtual void activateGimmick(EffectTarget& target) = 0;

    // 등장해 있는 동안 시간이 흐를 때마다 불린다. 지속 효과가 있는 보스가 오버라이드한다.
    virtual void onUpdate(int elapsedMs);

    // 등장·퇴장할 때 불린다. 남아 있는 기믹 효과를 지워야 하는 보스가 오버라이드한다.
    virtual void resetGimmick();

private:
    std::string name;
    int periodMs;
    int timerMs;
    bool active;
};
