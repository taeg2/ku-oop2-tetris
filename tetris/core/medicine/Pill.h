#pragma once
#include <memory>
#include <string>
#include "medicine/PillType.h"
#include "play/PlayContext.h"

class Inventory;

// 약(플레이어 보조 아이템)의 추상 클래스. 먹었을 때의 효과(use)만 자식마다 다르다.
//     use()는 약을 꺼낸 가방도 받는다. 효과로 새 약을 얻으면 그 가방에 넣는다.
class Pill {
public:
    explicit Pill(PillType type) : m_type(type) {}
    virtual ~Pill() = default;

    virtual bool canUse(const PlayContext& ctx) const;
    virtual void use(PlayContext& ctx, Inventory& bag) = 0;
    virtual std::string name() const = 0;
    PillType type() const { return m_type; }

    static std::unique_ptr<Pill> create(PillType type);  // 팩토리

protected:
    PillType m_type;
};
