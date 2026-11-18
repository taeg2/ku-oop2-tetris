#include "flow/AlbumScene.h"
#include <algorithm>
#include "common/TextUtil.h"
#include "stage/Album.h"
#include "common/GameConfig.h"
#include "stage/StageCatalog.h"
#include "ui/ISoundPlayer.h"
#include "ui/PhotoPainter.h"
#include "flow/EndingScene.h"
#include "flow/PlayScene.h"

namespace {
constexpr int kShowcaseMs = 1800;
constexpr int kInsertMs = 1100;
constexpr int kSlotTop = 6;
constexpr int kSlotWidth = 29;
constexpr Rect kPage{5, 3, 90, 16};

Point slotOrigin(int index) {
    return {kPage.x + 4 + index * kSlotWidth, kSlotTop};
}
}  // namespace

AlbumScene::AlbumScene(GameServices& services, int stageIndex)
    : Scene(services), m_stageIndex(stageIndex), m_caption({10, 21}, 80, 6) {
    m_caption.setFramed(true, Color::DarkYellow);
    m_caption.setTypingSpeed(config::kTypingCharsPerSecond);
}

void AlbumScene::onEnter() {
    m_services.sound.play(SoundId::StageClear);
}

void AlbumScene::handle(Command command) {
    if (command != Command::Confirm && command != Command::HardDrop) {
        return;
    }
    if (m_phase != Phase::Caption) {
        m_phase = Phase::Caption;  // 연출 건너뛰기
        m_phaseMs = 0;
        m_caption.setText("앨범 " + std::to_string(m_stageIndex + 1) + "장",
                          m_services.catalog.get(m_stageIndex).photo.albumCaption());
        return;
    }
    if (!m_caption.isFullyShown()) {
        m_caption.showAll();
        return;
    }
    goNext();
}

void AlbumScene::goNext() {
    m_services.sound.play(SoundId::Select);
    if (m_stageIndex + 1 < m_services.catalog.count()) {
        changeTo(std::make_unique<PlayScene>(m_services, m_stageIndex + 1));
    } else {
        changeTo(std::make_unique<EndingScene>(m_services));
    }
}

void AlbumScene::update(int dtMs) {
    m_phaseMs += dtMs;
    m_caption.update(dtMs);
    if (m_phase == Phase::Showcase && m_phaseMs >= kShowcaseMs) {
        m_phase = Phase::Insert;
        m_phaseMs = 0;
    } else if (m_phase == Phase::Insert && m_phaseMs >= kInsertMs) {
        m_phase = Phase::Caption;
        m_phaseMs = 0;
        m_caption.setText("앨범 " + std::to_string(m_stageIndex + 1) + "장",
                          m_services.catalog.get(m_stageIndex).photo.albumCaption());
    }
}

void AlbumScene::render(Console& console) const {
    console.fill({0, 0, console.width(), 1}, Color::DarkYellow);
    console.drawCentered(0, "사진 한 장을 되찾았습니다", Color::Black, Color::DarkYellow);
    if (m_phase == Phase::Showcase) {
        renderShowcase(console);
        return;
    }
    renderAlbum(console);
    if (m_phase == Phase::Caption) {
        m_caption.draw(console);
        drawHelp(console, "Enter/Space: 다음");
    }
}

void AlbumScene::renderShowcase(Console& console) const {
    const PhotoArt& photo = m_services.catalog.get(m_stageIndex).photo;
    const Point origin{(console.width() - PhotoArt::kWidth * 2) / 2, 4};
    console.fill({origin.x - 3, origin.y - 1, PhotoArt::kWidth * 2 + 6, PhotoArt::kHeight + 3}, Color::White);
    PhotoPainter::paint(console, origin, photo);
    console.drawCentered(origin.y + PhotoArt::kHeight + 1, photo.title(), Color::Black, Color::White);
    drawHelp(console, "Enter: 건너뛰기");
}

void AlbumScene::renderAlbum(Console& console) const {
    console.fill(kPage, Color::DarkYellow);
    console.draw({kPage.x + 3, kPage.y + 1}, "우리 가족 앨범", Color::Black, Color::DarkYellow);

    const auto& photos = m_services.album.photos();
    const int total = m_services.album.capacity();
    for (int i = 0; i < total; ++i) {
        Point slot = slotOrigin(i);
        const Rect paper{slot.x - 2, slot.y - 1, 28, 10};
        if (i >= static_cast<int>(photos.size())) {
            console.fill(paper, Color::DarkGray);
            console.draw({slot.x + 11, slot.y + 3}, "?", Color::Gray, Color::DarkGray);
            continue;
        }
        if (i == static_cast<int>(photos.size()) - 1 && m_phase == Phase::Insert) {
            const double t = std::min(1.0, static_cast<double>(m_phaseMs) / kInsertMs);
            const int startY = console.height();
            slot.y = startY + static_cast<int>((kSlotTop - startY) * (1 - (1 - t) * (1 - t)));
        }
        const Rect moving{slot.x - 2, slot.y - 1, 28, 10};
        console.fill(moving, Color::White);
        PhotoPainter::paintThumbnail(console, slot, photos[static_cast<std::size_t>(i)]);
        const std::string& title = photos[static_cast<std::size_t>(i)].title();
        console.draw({slot.x + (24 - TextUtil::width(title)) / 2, slot.y + 9}, title, Color::Black, Color::DarkYellow);
    }
}
