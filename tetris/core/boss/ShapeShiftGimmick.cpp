#include "boss/ShapeShiftGimmick.h"
#include "board/Board.h"
#include "block/Tetromino.h"
#include "play/EventBus.h"

ShapeShiftGimmick::ShapeShiftGimmick(int intervalMs, int warningMs) : PeriodicGimmick(intervalMs, warningMs) {}

void ShapeShiftGimmick::trigger(PlayContext& ctx) {
    m_pending = true;
    morph(ctx);
}

void ShapeShiftGimmick::onPieceSpawn(PlayContext& ctx) {
    if (m_pending) {
        morph(ctx);
    }
}

void ShapeShiftGimmick::morph(PlayContext& ctx) {
    const auto currentType = static_cast<int>(ctx.current.type());
    std::uniform_int_distribution<int> offset(1, kTetrominoTypeCount - 1);
    const auto newType = static_cast<TetrominoType>((currentType + offset(ctx.rng)) % kTetrominoTypeCount);

    if (const auto placed = findPlacement(ctx.board, ctx.current.morphed(newType))) {
        ctx.current = *placed;
        m_pending = false;
        ctx.bus.publish(GameEvent::Type::BossTriggered);
    }
}

std::optional<Tetromino> ShapeShiftGimmick::findPlacement(const Board& board, const Tetromino& morphed) {
    for (Point kick : {Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{0, -1}}) {
        if (board.canPlace(morphed, kick)) {
            return morphed.moved(kick);
        }
    }
    return std::nullopt;  // 바꿀 자리가 없으면 다음 블록이 나올 때 다시 시도한다
}

std::string ShapeShiftGimmick::statusText() const {
    if (m_pending) {
        return "다음 블록 뒤바뀜";
    }
    if (isWarning()) {
        return "곧 블록이 뒤바뀜!";
    }
    return "뒤바뀜까지 " + std::to_string(secondsLeft(remainingMs())) + "초";
}
