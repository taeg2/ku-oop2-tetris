#pragma once

#include <memory>
#include <string>

#include "play/EffectTarget.h"

// 약(플레이어 보조 아이템)의 추상 클래스.
// 약이 실제로 하는 일(applyTo)만 자식 클래스가 정한다.
// 새 약은 이 클래스를 상속한 클래스를 추가하기만 하면 된다. (OCP)
//
// 약은 EffectTarget(추상 클래스)으로만 게임에 효과를 건다. (DIP)
class Medicine {
public:
    virtual ~Medicine();

    // ===== 쿼리 =====

    // 약 이름 (기획안에서 아직 가칭이라 main에서 넘겨준다)
    std::string getName() const;

    // 같은 종류·설정을 가진 새 약을 만든다. 구체 클래스를 몰라도 약을 복사할 수 있게 해 준다.
    virtual std::unique_ptr<Medicine> clone() const = 0;

    // ===== 커맨드 =====

    // 약 효과를 건다.
    virtual void applyTo(EffectTarget& target) = 0;

protected:
    explicit Medicine(const std::string& name);

private:
    std::string name;
};
