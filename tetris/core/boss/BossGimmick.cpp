#include "boss/BossGimmick.h"

// 기본 구현은 아무것도 하지 않는다. 필요한 기믹만 재정의한다.
void BossGimmick::onActivate(PlayContext&) {}
void BossGimmick::onDeactivate(PlayContext&) {}
void BossGimmick::onTick(PlayContext&, int) {}
void BossGimmick::onPieceSpawn(PlayContext&) {}
