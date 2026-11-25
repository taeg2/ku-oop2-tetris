#include "flow/StoryArt.h"
#include "common/GameConfig.h"
#include "stage/StageCatalog.h"

namespace {
const Point kArtOrigin{layout::kArtX, layout::kArtY};
}  // namespace

StoryArt::StoryArt(const StageCatalog& catalog, ISoundPlayer& sound, std::mt19937& rng)
    : m_catalog(catalog), m_sound(sound), m_rng(rng) {
    restart();
}

void StoryArt::restart() {
    m_stageArt.reset();  // 세션을 읽으므로 세션보다 먼저 지운다
    m_session.reset();
    m_stageIndex = -1;
    m_roundInStage = 0;
    m_lastCleared = -1;
    m_roundEndPending = false;
    m_remembered.assign(MemoryBookView::kSlots, false);

    m_house = std::make_unique<PrologueView>(kArtOrigin);
    m_album = std::make_unique<AlbumView>(kArtOrigin);
    m_menu = std::make_unique<MenuView>(kArtOrigin);
    m_settings = std::make_unique<SettingsView>(kArtOrigin);
    m_book = std::make_unique<MemoryBookView>(kArtOrigin);
    m_ending = std::make_unique<EndingView>(kArtOrigin);
}

void StoryArt::beginStage(int stageIndex) {
    if (m_session && m_stageIndex == stageIndex && !m_session->isCleared()) {
        if (m_session->isFailed()) {
            m_session->restartBoard();  // 재도전: 지금 라운드부터 다시
        }
        return;
    }
    if (m_session) {
        m_session->changeStage(m_catalog.get(stageIndex));  // 그림(StageArtView)은 같은 세션을 계속 읽는다
    } else {
        m_session = std::make_unique<StageSession>(m_catalog.get(stageIndex), m_sound, m_rng);
        m_stageArt = std::make_unique<StageArtView>(kArtOrigin, *m_session, config::kLinesPerPiece);
        m_session->subscribe(*this);
        m_session->subscribe(*m_stageArt);
    }
    m_stageIndex = stageIndex;
    m_roundInStage = 0;
    m_lastCleared = -1;
    m_roundEndPending = false;
}

void StoryArt::clearRoundEnd() {
    m_roundEndPending = false;
}

void StoryArt::onEvent(const GameEvent& event) {
    if (event.type != GameEvent::Type::PieceFilled || m_roundInStage >= kRoundsPerStage) {
        return;
    }
    m_lastCleared = m_roundInStage;
    m_remembered[static_cast<std::size_t>(m_stageIndex * kRoundsPerStage + m_roundInStage)] = true;
    ++m_roundInStage;
    m_roundEndPending = true;
}

