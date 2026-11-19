#include "ui/StageArtView.h"
#include "ui/BeachRound.h"
#include "ui/BedroomRound.h"
#include "ui/KitchenRound.h"
#include "ui/LakeRound.h"
StageArtView::StageArtView(Point origin, const StageSession& session, int linesPerRound) : View(origin), m_state(session), m_linesPerRound(linesPerRound) {
    m_rounds.push_back(std::make_unique<LakeRound>());
    m_rounds.push_back(std::make_unique<KitchenRound>());
    m_rounds.push_back(std::make_unique<BeachRound>());
    m_rounds.push_back(std::make_unique<BedroomRound>());
    for (auto& r : m_rounds) r->reset();
}
void StageArtView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) {
        m_accMs -= kFrameMs;
        ++m_t;
        if (m_state.clearing()) { if (m_flashT < 0) m_flashT = m_t; } else m_flashT = -1;
        round().update(m_state, m_t);
        if (m_advance && round().endingDone()) {
            m_advance = false;
            m_lines = 0; m_visual = 0;
            if (m_round + 1 < static_cast<int>(m_rounds.size())) ++m_round;
            round().reset();
        }
    }
}
void StageArtView::onEvent(const GameEvent& event) {
    if (event.type == GameEvent::Type::LinesCleared) {
        m_lines += event.value;
        // 실제 줄 수를 그림 진행 칸 수(goal)로 비율 환산한다. 예: 3줄로 끝나는 라운드도 그림은 10칸 진행
        int goal = round().goal(), visual = std::min(goal, (m_lines * goal + m_linesPerRound - 1) / m_linesPerRound);
        if (visual <= m_visual) return;
        std::vector<int> rows;
        for (int i = 0; i < visual - m_visual; ++i) rows.push_back(std::min(PlayStateReader::kRows - 1, event.row + i % std::max(1, event.value)));
        m_visual = visual;
        round().onLinesCleared(rows, m_visual, m_t);
    } else if (event.type == GameEvent::Type::PieceFilled) {
        m_advance = true;  // 끝 연출이 끝나면 update()에서 다음 라운드로 넘긴다
    }
}
bool StageArtView::wantsPause() const { return m_advance || !round().playing(m_t); }
void StageArtView::render() const {
    resetPalette();
    std::fill(m_frame.px.begin(), m_frame.px.end(), 0);
    int k = m_flashT < 0 ? -1 : std::min(2, m_t - m_flashT);
    round().draw(m_frame, m_state, m_visual, m_t, k);
}
void StageArtView::draw(Console& console) const {
    if (m_drawnT != m_t) {
        m_drawnT = m_t;
        render();
        m_sixel = m_encoder.encode(m_frame, kScale);
        console.drawImage(m_origin, m_sixel);
    }
}
