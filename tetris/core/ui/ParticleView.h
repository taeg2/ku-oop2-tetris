#pragma once
#include <vector>
#include "ui/View.h"

// 지운 줄의 블록 조각이 사진 틀로 날아가는 연출 (기획안 3장 "조각 연출")
class ParticleView : public View {
public:
    ParticleView();
    void spawn(Point from, Point to, Color color, int delayMs);
    void update(int dtMs) override;
    void draw(Console& console) const override;

private:
    struct Particle {
        Point from;
        Point to;
        Color color;
        int ageMs;
    };
    inline static const int kLifeMs = 550;
    std::vector<Particle> m_particles;
};
