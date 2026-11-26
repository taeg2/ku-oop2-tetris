#pragma once
#include <map>
#include <memory>
#include <vector>
#include "play/IGameEventListener.h"
#include "medicine/Pill.h"
using namespace std;

// 보유한 약. 종류별 한도가 있고, 보스가 나타나면(사진 2/4) 파란 약 하나를 받는다.
class Inventory : public IGameEventListener {
public:
    explicit Inventory(int limitPerType);

    void onEvent(const GameEvent& event) override;
    bool canAdd(PillType type) const;
    void add(PillType type);                 // 가득 찼으면 아무것도 하지 않는다
    void add(unique_ptr<Pill> pill);
    bool canUse(PillType type, const PlayContext& ctx) const;
    void use(PillType type, PlayContext& ctx);  // canUse()가 false면 아무것도 하지 않는다
    int count(PillType type) const;
    void clear();

private:
    int m_limit;
    map<PillType, vector<unique_ptr<Pill>>> m_slots;
};
