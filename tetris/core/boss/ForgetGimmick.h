#pragma once
#include <vector>
#include "common/Point.h"
#include "boss/PeriodicGimmick.h"

// STAGE 3 보스 "지우개" - 까먹음.
//     쌓인 블록 몇 칸을 골라 깜빡이게 한 뒤(경고), 시간이 되면 지워 버린다.
class ForgetGimmick : public PeriodicGimmick {
public:
    ForgetGimmick(int intervalMs, int warningMs, int cellsPerTrigger);

    void onDeactivate(PlayContext& ctx) override;
    std::string statusText() const override;

protected:
    void onWarning(PlayContext& ctx) override;
    void trigger(PlayContext& ctx) override;

private:
    int m_cellsPerTrigger;
    std::vector<Point> m_targets;
};
