#include "ui/ParticleView.h"
#include <algorithm>

ParticleView::ParticleView() : View(Point{0, 0}) {}

void ParticleView::spawn(Point from, Point to, Color color, int delayMs) {
    m_particles.push_back({from, to, color, -delayMs});
}

void ParticleView::update(int dtMs) {
    View::update(dtMs);
    for (Particle& p : m_particles) {
        p.ageMs += dtMs;
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
                                     [](const Particle& p) { return p.ageMs >= kLifeMs; }),
                      m_particles.end());
}

void ParticleView::draw(Console& console) const {
    for (const Particle& p : m_particles) {
        if (p.ageMs < 0) {
            continue;
        }
        const double t = static_cast<double>(p.ageMs) / kLifeMs;
        const double eased = t * t;  // 처음엔 천천히, 갈수록 빠르게
        const int x = p.from.x + static_cast<int>((p.to.x - p.from.x) * eased);
        const int y = p.from.y + static_cast<int>((p.to.y - p.from.y) * eased) - static_cast<int>(4 * t * (1 - t) * 4);
        console.fill({x, y, 2, 1}, p.color);
    }
}
