#include "medicine/Inventory.h"
using namespace std;

Inventory::Inventory(int limitPerType) : m_limit(limitPerType) {}

void Inventory::onEvent(const GameEvent& event) {
    if (event.type == GameEvent::Type::PhotoHalf) {
        add(PillType::Blue);
    }
}

bool Inventory::canAdd(PillType type) const {
    return count(type) < m_limit;
}

void Inventory::add(PillType type) {
    add(Pill::create(type));
}

void Inventory::add(unique_ptr<Pill> pill) {
    if (pill && canAdd(pill->type())) {
        m_slots[pill->type()].push_back(std::move(pill));
    }
}

bool Inventory::canUse(PillType type, const PlayContext& ctx) const {
    const auto it = m_slots.find(type);
    return it != m_slots.end() && !it->second.empty() && it->second.back()->canUse(ctx);
}

void Inventory::use(PillType type, PlayContext& ctx) {
    if (!canUse(type, ctx)) {
        return;
    }
    auto& slot = m_slots[type];
    unique_ptr<Pill> taken = std::move(slot.back());
    slot.pop_back();
    taken->use(ctx, *this);
}

int Inventory::count(PillType type) const {
    const auto it = m_slots.find(type);
    return it == m_slots.end() ? 0 : static_cast<int>(it->second.size());
}

void Inventory::clear() {
    m_slots.clear();
}
