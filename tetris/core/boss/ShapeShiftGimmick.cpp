#include "boss/ShapeShiftGimmick.h"
#include <algorithm>
#include <array>
#include "board/Board.h"
#include "block/Tetromino.h"
#include "play/EventBus.h"
using namespace std;

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
    uniform_int_distribution<int> offset(1, kTetrominoTypeCount - 1);
    const auto newType = static_cast<TetrominoType>((currentType + offset(ctx.rng)) % kTetrominoTypeCount);

    Tetromino placed = ctx.current;
    if (findPlacement(ctx.board, ctx.current.morphed(newType), placed)) {
        ctx.current = placed;
        m_pending = false;
        ctx.bus.publish(GameEvent::Type::BossTriggered);
    }
}

bool ShapeShiftGimmick::findPlacement(const Board& board, const Tetromino& morphed, Tetromino& result) {
    const array<Point, 4> kicks = {Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{0, -1}};
    const auto found = find_if(kicks.begin(), kicks.end(),
                                    [&](Point kick) { return board.canPlace(morphed, kick); });
    if (found == kicks.end()) {
        return false;  // 바꿀 자리가 없으면 다음 블록이 나올 때 다시 시도한다
    }
    result = morphed.moved(*found);
    return true;
}

string ShapeShiftGimmick::statusText() const {
    if (m_pending) {
        return "다음 블록 뒤바뀜";
    }
    if (isWarning()) {
        return "곧 블록이 뒤바뀜!";
    }
    return "뒤바뀜까지 " + to_string(secondsLeft(remainingMs())) + "초";
}
