#include "ui/AlbumView.h"
#include <algorithm>
#include "ui/BeachRound.h"
#include "ui/ClassroomRound.h"
#include "ui/BedroomRound.h"
#include "ui/KitchenRound.h"
#include "ui/LakeRound.h"
#include "ui/SoccerRound.h"
#include "ui/UniRound.h"
#include "ui/InterviewRound.h"
#include "ui/OfficeRound.h"
#include "ui/WeddingRound.h"
#include "ui/NurseryRound.h"
#include "ui/FadingRound.h"
using namespace std;

AlbumView::AlbumView(Point origin) : View(origin) {
    m_previews.push_back(make_unique<LakeRound>());
    m_previews.push_back(make_unique<KitchenRound>());
    m_previews.push_back(make_unique<BeachRound>());
    m_previews.push_back(make_unique<BedroomRound>());
    m_previews.push_back(make_unique<ClassroomRound>());
    m_previews.push_back(make_unique<SoccerRound>());
    m_previews.push_back(make_unique<UniRound>());
    m_previews.push_back(make_unique<InterviewRound>());
    m_previews.push_back(make_unique<OfficeRound>());
    m_previews.push_back(make_unique<WeddingRound>());
    m_previews.push_back(make_unique<NurseryRound>());
    m_previews.push_back(make_unique<FadingRound>());
}
void AlbumView::open(int stage, int clearedRound) {
    m_album.setStage(stage);
    m_album.open(clearedRound, m_t);
    m_pick = -1;
    m_stageFinished = false;
}
int AlbumView::takePick() { int p = m_pick; m_pick = -1; return p; }
void AlbumView::handle(Command command) {
    if (!m_album.isOpen() || m_album.flipping()) return;
    switch (command) {
        case Command::MoveLeft: m_album.left(); break;
        case Command::MoveRight: m_album.right(); break;
        case Command::Rotate: m_album.up(); break;
        case Command::SoftDrop: m_album.down(); break;
        case Command::Pause: if (m_album.inDetail()) m_album.closeDetail(); break;
        case Command::Confirm:
        case Command::HardDrop:
            if (!m_album.inDetail()) m_album.openDetail();
            else if (m_album.cancelChosen()) m_album.closeDetail();
            else if (m_album.playChosen()) { m_pick = m_album.stage() * AlbumScreen::SLOTS + m_album.selected(); m_album.close(); }
            break;
        default: break;
    }
}
void AlbumView::update(int dtMs) {
    View::update(dtMs);
    m_accMs += dtMs;
    while (m_accMs >= kFrameMs) {
        m_accMs -= kFrameMs;
        ++m_t;
        if (!m_album.isOpen()) continue;
        // 스테이지 네 칸이 다 차면 4초 뒤 다음 페이지로 넘긴다
        if (m_album.stage() < AlbumScreen::STAGES - 1 && m_album.stageComplete() && !m_album.flipping() && m_t - m_album.openedAt() >= 40) m_album.startFlip(m_t);
        if (m_album.flipDone(m_t)) { m_album.close(); m_album.endFlip(); m_stageFinished = true; }
    }
}
void AlbumView::render() const {
    resetPalette();
    fill(m_frame.px.begin(), m_frame.px.end(), 0);
    if (m_album.inDetail()) {
        int want = m_album.stage() * AlbumScreen::SLOTS + m_album.selected();
        if (want < static_cast<int>(m_previews.size()) && m_previews[static_cast<size_t>(want)]) {
            if (m_shown != want) { m_shown = want; m_previews[static_cast<size_t>(want)]->reset(); }
            fill(m_preview.px.begin(), m_preview.px.end(), 0);
            m_previews[static_cast<size_t>(want)]->draw(m_preview, m_empty, 0, 40 + m_t % 400);
            m_album.draw(m_frame, m_t, &m_preview);
            return;
        }
    }
    m_album.draw(m_frame, m_t);
}
void AlbumView::draw(Console& console) const {
    if (!m_album.isOpen() || m_drawnT == m_t) return;
    m_drawnT = m_t;
    render();
    m_sixel = m_encoder.encode(m_frame, kScale);
    console.drawImage(m_origin, m_sixel);
}
