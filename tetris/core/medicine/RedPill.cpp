#include "medicine/RedPill.h"
#include "board/Board.h"
#include "play/EventBus.h"
#include "common/GameConfig.h"
#include "stage/PhotoProgress.h"

RedPill::RedPill() : Pill(PillType::Red) {}

bool RedPill::canUse(const PlayContext& ctx) const {
    return !ctx.board.isRowEmpty(Board::kHeight - 1);
}

void RedPill::use(PlayContext& ctx) {
    ctx.board.clearRow();
    ctx.bus.publish(GameEvent::Type::PillUsed, static_cast<int>(m_type));
    if (config::kRedPillCountsAsLine) {
        ctx.bus.publish(GameEvent{GameEvent::Type::LinesCleared, 1, Board::kHeight - 1});
        ctx.progress.addLines(1);
    }
}
