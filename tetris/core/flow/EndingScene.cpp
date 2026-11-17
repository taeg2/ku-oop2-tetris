#include "flow/EndingScene.h"
#include <algorithm>
#include "common/TextUtil.h"
#include "stage/StageCatalog.h"
#include "ui/ISoundPlayer.h"
#include "ui/PhotoPainter.h"
#include "flow/TitleScene.h"

namespace {
constexpr Point kPhotoOrigin{26, 3};
constexpr int kBlockPixels = 4;
constexpr int kFragmentDelayMs = 650;
constexpr int kFragmentMoveMs = 700;

enum Line : std::size_t {
    kOpenAlbum = 0, kPhoto1, kPhoto2, kPhoto3, kReassemble, kGrowing, kNow, kChildAsks, kFatherAnswers,
    kFatherNames, kTheEnd
};
}  // namespace

EndingScene::EndingScene(GameServices& services)
    : NarrativeScene(services, {
          {"", "현재. 아버지가 완성된 앨범을 천천히 펼치십니다."},
          {"앨범 1장", services.catalog.get(0).photo.albumCaption()},
          {"앨범 2장", services.catalog.get(1).photo.albumCaption()},
          {"앨범 3장", services.catalog.get(2).photo.albumCaption()},
          {"", "마지막 사진이 테트로미노 조각으로 흩어졌다가, 하나씩 제자리를 찾아갑니다."},
          {"", "사진 속 아기는 어린이가 되고, 학생이 되고, 어른이 되어..."},
          {"", "지금, 아버지 곁에 앉아 있습니다."},
          {"자녀", "\"이날 기억나요, 아버지?\""},
          {"아버지", "\"응. 네가 너무 작아서, 어떻게 안아야 할지 몰랐지.\""},
          {"아버지", "\"...우리 민준이. 이렇게 컸구나.\""},
          {"", "기억을 맞추는 테트리스 - 끝.  함께해 주셔서 고맙습니다."},
      }) {
    // 24x16 사진을 6x4 블록으로 나누고, 그 24블록을 테트로미노 6개로 나눈 배치
    //   A A A B B B
    //   A C C C B D
    //   E C F F D D
    //   E E E F F D
    m_fragments = {
        {{{0, 0}, {1, 0}, {2, 0}, {0, 1}}, {-24, -2}},  // L
        {{{3, 0}, {4, 0}, {5, 0}, {4, 1}}, {22, -2}},   // T
        {{{1, 1}, {2, 1}, {3, 1}, {1, 2}}, {-32, 1}},    // J
        {{{5, 1}, {4, 2}, {5, 2}, {5, 3}}, {34, 0}},     // T
        {{{0, 2}, {0, 3}, {1, 3}, {2, 3}}, {-20, 2}},   // J
        {{{2, 2}, {3, 2}, {3, 3}, {4, 3}}, {16, 2}},    // Z
    };
}

const PhotoArt& EndingScene::photo(int index) const {
    return m_services.catalog.get(index).photo;
}

void EndingScene::onLineShown(std::size_t index) {
    if (index == kReassemble) {
        m_assembleMs = 0;
    }
}

void EndingScene::onUpdate(int dtMs) {
    if (m_assembleMs >= 0) {
        const int before = m_assembleMs / kFragmentDelayMs;
        m_assembleMs += dtMs;
        const int after = m_assembleMs / kFragmentDelayMs;
        if (after != before && after <= static_cast<int>(m_fragments.size())) {
            m_services.sound.play(SoundId::Lock);
        }
    }
}

bool EndingScene::canAdvance() const {
    if (lineIndex() != kReassemble) {
        return true;
    }
    const int total = static_cast<int>(m_fragments.size()) * kFragmentDelayMs + kFragmentMoveMs;
    return m_assembleMs >= total;
}

void EndingScene::renderFrame(Console& console, const std::string& caption) const {
    console.fill({kPhotoOrigin.x - 3, kPhotoOrigin.y - 1, PhotoArt::kWidth * 2 + 6, PhotoArt::kHeight + 3},
                 Color::White);
    console.drawCentered(kPhotoOrigin.y + PhotoArt::kHeight, caption, Color::Black, Color::White);
}

void EndingScene::renderBackground(Console& console) const {
    const std::size_t line = lineIndex();
    if (line == kOpenAlbum) {
        renderAlbumSpread(console);
    } else if (line >= kPhoto1 && line <= kPhoto3) {
        const int index = static_cast<int>(line - kPhoto1);
        renderFrame(console, photo(index).title());
        PhotoPainter::paint(console, kPhotoOrigin, photo(index));
    } else if (line == kReassemble) {
        renderReassembly(console);
    } else if (line == kGrowing) {
        const int index = std::min(2, timeInLineMs() / 1100);  // 어린이 -> 학생 -> 어른
        renderFrame(console, photo(index).title());
        PhotoPainter::paint(console, kPhotoOrigin, photo(index));
    } else {
        renderFrame(console, photo(2).title());
        PhotoPainter::paint(console, kPhotoOrigin, photo(2));
        if (line == kTheEnd) {
            console.fill({0, 0, console.width(), 1}, Color::DarkYellow);
            console.drawCentered(0, "THE END", Color::Black, Color::DarkYellow);
        }
    }
}

void EndingScene::renderAlbumSpread(Console& console) const {
    console.fill({5, 3, 90, 16}, Color::DarkYellow);
    console.draw({8, 4}, "우리 가족 앨범", Color::Black, Color::DarkYellow);
    for (int i = 0; i < 3; ++i) {
        const Point slot{9 + i * 29, 7};
        console.fill({slot.x - 2, slot.y - 1, 28, 10}, Color::White);
        PhotoPainter::paintThumbnail(console, slot, photo(i));
        console.draw({slot.x + (24 - TextUtil::width(photo(i).title())) / 2, slot.y + 9}, photo(i).title(),
                     Color::Black, Color::DarkYellow);
    }
}

void EndingScene::renderReassembly(Console& console) const {
    const PhotoArt& last = photo(2);
    renderFrame(console, "");
    for (std::size_t i = 0; i < m_fragments.size(); ++i) {
        const int start = static_cast<int>(i) * kFragmentDelayMs;
        const double t = std::clamp(static_cast<double>(m_assembleMs - start) / kFragmentMoveMs, 0.0, 1.0);
        const double eased = 1.0 - (1.0 - t) * (1.0 - t);
        const Fragment& f = m_fragments[i];
        const Point offset{static_cast<int>(f.scatter.x * (1.0 - eased)), static_cast<int>(f.scatter.y * (1.0 - eased))};
        const PhotoTone tone = t >= 1.0 ? PhotoTone::Vivid : PhotoTone::Faded;
        for (Point block : f.blocks) {
            const Rect pixels{block.x * kBlockPixels, block.y * kBlockPixels, kBlockPixels, kBlockPixels};
            const Point origin{kPhotoOrigin.x + pixels.x * 2 + offset.x, kPhotoOrigin.y + pixels.y + offset.y};
            PhotoPainter::paint(console, origin, last, pixels, tone);
        }
    }
}

void EndingScene::onNarrationEnd() {
    changeTo(std::make_unique<TitleScene>(m_services));
}
