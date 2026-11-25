#pragma once
#include <vector>
#include "stage/StageConfig.h"
using namespace std;

// 3개 스테이지 데이터 목록 (기획안 4, 5장)
class StageCatalog {
public:
    StageCatalog();
    const StageConfig& get(int index) const;
    int count() const { return static_cast<int>(m_stages.size()); }

private:
    vector<StageConfig> m_stages;
};
