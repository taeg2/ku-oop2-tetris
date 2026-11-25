#pragma once
#include "block/Tetromino.h"
#include "boss/PeriodicGimmick.h"

class Board;

// STAGE 2 보스 "엉킨 실타래" - 착각(인지 혼동).
//     떨어지던 블록이 갑자기 다른 모양으로 바뀐다. 바꿀 자리가 없으면 다음 블록이 나올 때 바꾼다.
class ShapeShiftGimmick : public PeriodicGimmick {
public:
    ShapeShiftGimmick(int intervalMs, int warningMs);

    void onPieceSpawn(PlayContext& ctx) override;
    std::string statusText() const override;

protected:
    void trigger(PlayContext& ctx) override;

private:
    void morph(PlayContext& ctx);  // 성공하면 m_pending을 내린다
    // 바뀐 블록을 놓을 자리를 찾으면 result를 채우고 true를 돌려준다
    static bool findPlacement(const Board& board, const Tetromino& morphed, Tetromino& result);

    bool m_pending = false;
};
