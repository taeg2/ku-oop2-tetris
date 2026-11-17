#include "flow/PlayScene.h"
#include "common/TextUtil.h"
#include "stage/Album.h"
#include "common/GameConfig.h"
#include "stage/StageCatalog.h"
#include "ui/ISoundPlayer.h"
#include "ui/BoardView.h"
#include "ui/HudView.h"
#include "ui/NextBlockView.h"
#include "ui/PhotoFrameView.h"
#include "flow/AlbumScene.h"
#include "flow/GameOverScene.h"
#include "flow/TitleScene.h"

namespace {
const Point kPhotoCenter{layout::kPhotoX + PhotoFrameView::kWidthColumns / 2,
                         layout::kPhotoY + PhotoFrameView::kHeightRows / 2};
const Rect kOverlay{25, 9, 50, 9};
}  // namespace

PlayScene::PlayScene(GameServices& services, int stageIndex)
    : Scene(services),
      m_stageIndex(stageIndex),
      m_config(services.catalog.get(stageIndex)),
      m_session(m_config, services.sound, services.rng),
      m_message({layout::kMessageX, layout::kMessageY}, layout::kMessageWidth, layout::kMessageHeight) {
    m_views.push_back(std::make_unique<BoardView>(Point{layout::kBoardX, layout::kBoardY}, m_session));
    m_views.push_back(std::make_unique<NextBlockView>(Point{layout::kNextX, layout::kNextY}, m_session));
    m_views.push_back(std::make_unique<HudView>(Point{layout::kHudX, layout::kHudY}, m_session));
    m_views.push_back(std::make_unique<PhotoFrameView>(Point{layout::kPhotoX, layout::kPhotoY}, m_session));
    m_message.setTypingSpeed(config::kTypingCharsPerSecond * 2);
    m_session.subscribe(*this);
}

void PlayScene::onEnter() {
    showMessage(m_config.period + "의 기억",
                "줄을 3개 지울 때마다 사진 조각이 하나 돌아옵니다. 블록 사이로 " + m_config.blockTheme +
                    "... 그 시절의 물건들이 떠오릅니다.");
}

void PlayScene::setPhase(Phase phase) {
    m_phase = phase;
    m_phaseMs = 0;
}

void PlayScene::showMessage(const std::string& speaker, const std::string& text) {
    m_message.setText(speaker, text);
}

void PlayScene::handle(Command command) {
    switch (m_phase) {
        case Phase::Intro:
            if (command == Command::Confirm || command == Command::HardDrop) {
                setPhase(Phase::Playing);
            }
            break;
        case Phase::Paused:
            if (command == Command::Pause || command == Command::Confirm) {
                setPhase(Phase::Playing);
            } else if (command == Command::Quit) {
                changeTo(std::make_unique<TitleScene>(m_services));
            }
            break;
        case Phase::Playing:
            if (command == Command::Pause || command == Command::Quit) {
                setPhase(Phase::Paused);
            } else {
                m_session.handle(command);
            }
            break;
        default:
            break;
    }
}

void PlayScene::update(int dtMs) {
    m_phaseMs += dtMs;
    if (m_phase == Phase::Paused) {
        return;
    }
    for (auto& view : m_views) {
        view->update(dtMs);
    }
    m_message.update(dtMs);
    m_particles.update(dtMs);

    switch (m_phase) {
        case Phase::Intro:
            if (m_phaseMs >= config::kStageIntroMs) {
                setPhase(Phase::Playing);
            }
            break;
        case Phase::Playing:
            m_session.update(dtMs);
            if (m_session.isCleared()) {
                setPhase(Phase::Completing);
            } else if (m_session.isFailed()) {
                setPhase(Phase::Failing);
            }
            break;
        case Phase::Completing:
            if (m_phaseMs >= config::kStageCompleteMs) {
                m_services.album.add(m_config.photo);
                changeTo(std::make_unique<AlbumScene>(m_services, m_stageIndex));
            }
            break;
        case Phase::Failing:
            if (m_phaseMs >= config::kStageFailMs) {
                changeTo(std::make_unique<GameOverScene>(m_services, m_stageIndex));
            }
            break;
        default:
            break;
    }
}

void PlayScene::onEvent(const GameEvent& event) {
    using Type = GameEvent::Type;
    switch (event.type) {
        case Type::LinesCleared:
            spawnClearParticles(event.row, event.value);
            break;
        case Type::PieceFilled:
            if (event.value < config::kPhotoPieces) {
                showMessage("기억 조각 " + std::to_string(event.value) + "/" + std::to_string(config::kPhotoPieces),
                            m_config.photo.pieceCaption(event.value - 1));
            }
            break;
        case Type::PhotoHalf:
            showMessage("보스 등장: " + m_session.boss().name(),
                        m_config.photo.pieceCaption(config::kBossAppearPieces - 1) + " 그런데... " +
                            m_session.boss().entranceLine() + " (약 가방에 파란 약을 하나 챙겨 두었다)");
            break;
        case Type::PillGained:
            showMessage("", static_cast<PillType>(event.value) == PillType::Blue
                                ? "블록 사이에서 파란 약을 찾았다. [1] 키로 먹을 수 있다."
                                : "블록 사이에서 빨간 약을 찾았다. [2] 키로 먹을 수 있다.");
            break;
        case Type::PillUsed:
            showMessage("", static_cast<PillType>(event.value) == PillType::Blue
                                ? "파란 약을 먹었다. 마음이 차분해지며 시간이 느리게 흐른다."
                                : "빨간 약을 먹었다. 가장 아래의 엉킨 기억이 정리된다.");
            break;
        case Type::PhotoComplete:
            showMessage("기억 조각 4/4",
                        m_config.photo.pieceCaption(config::kPhotoPieces - 1) + " 방해하던 그림자가 물러간다.");
            break;
        default:
            break;
    }
}

void PlayScene::spawnClearParticles(int topRow, int rowCount) {
    static const Color kColors[] = {Color::Yellow, Color::White, Color::Cyan, Color::Magenta};
    for (int r = 0; r < rowCount; ++r) {
        for (int x = 0; x < Board::kWidth; x += 2) {
            const Point from{layout::kBoardX + 2 + x * 2, layout::kBoardY + 1 + topRow + r};
            m_particles.spawn(from, kPhotoCenter, kColors[(x / 2 + r) % 4], x * 20 + r * 60);
        }
    }
}

void PlayScene::render(Console& console) const {
    renderHeader(console);
    for (const auto& view : m_views) {
        view->draw(console);
    }
    m_message.draw(console);
    m_particles.draw(console);
    drawHelp(console, "방향키: 이동/회전   Space: 바로 내리기   1,2: 약 먹기   Esc: 일시정지");
    renderOverlay(console);
}

void PlayScene::renderHeader(Console& console) const {
    console.fill({0, 0, console.width(), 1}, Color::DarkBlue);
    console.draw({2, 0}, "기억을 맞추는 테트리스", Color::White, Color::DarkBlue);
    const std::string right = m_config.title + "  " + m_config.period;
    console.draw({console.width() - 2 - TextUtil::width(right), 0}, right, Color::Yellow, Color::DarkBlue);
}

void PlayScene::renderOverlay(Console& console) const {
    switch (m_phase) {
        case Phase::Intro:
            drawDialog(console, kOverlay, Color::DarkYellow);
            console.drawCentered(kOverlay.y + 2, m_config.title, Color::Yellow);
            console.drawCentered(kOverlay.y + 4, m_config.period, Color::White);
            console.drawCentered(kOverlay.y + 6, "Enter: 바로 시작", Color::DarkGray);
            break;
        case Phase::Paused:
            drawDialog(console, kOverlay, Color::Gray);
            console.drawCentered(kOverlay.y + 2, "잠시 쉬어 갑니다", Color::White);
            console.drawCentered(kOverlay.y + 4, "Esc / Enter: 계속하기", Color::Gray);
            console.drawCentered(kOverlay.y + 6, "Q: 타이틀로", Color::DarkGray);
            break;
        case Phase::Failing:
            drawDialog(console, kOverlay, Color::DarkRed);
            console.drawCentered(kOverlay.y + 3, "기억이 흐려졌습니다...", Color::Red);
            break;
        default:
            break;
    }
}
