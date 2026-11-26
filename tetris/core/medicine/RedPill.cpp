#include "medicine/RedPill.h"
#include "board/Board.h"
#include "play/EventBus.h"
#include "common/GameConfig.h"
#include "medicine/Inventory.h"
#include "stage/PhotoProgress.h"
using namespace std;

RedPill::RedPill() : Pill(PillType::Red) {}

bool RedPill::canUse(const PlayContext& ctx) const {
    return !ctx.board.isRowEmpty(Board::kHeight - 1);
}

void RedPill::use(PlayContext& ctx, Inventory& bag) {
    const vector<PillType> pills = ctx.board.pillsInRows({Board::kHeight - 1});  // 지우기 전에 꺼낸다
    ctx.board.clearRow();
    ctx.bus.publish(GameEvent::Type::PillUsed, static_cast<int>(m_type));
    if (config::kRedPillCountsAsLine) {
        ctx.bus.publish(GameEvent{GameEvent::Type::LinesCleared, 1, Board::kHeight - 1});
        ctx.progress.addLines(1);
    }
    for (PillType pill : pills) {
        if (bag.canAdd(pill)) {
            bag.add(pill);
            ctx.bus.publish(GameEvent::Type::PillGained, static_cast<int>(pill));
        }
    }
}
