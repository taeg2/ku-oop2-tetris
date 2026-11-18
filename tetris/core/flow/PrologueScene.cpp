#include "flow/PrologueScene.h"
#include "stage/StageCatalog.h"
#include "ui/PhotoPainter.h"
#include "flow/PlayScene.h"

namespace {
constexpr std::size_t kPointingLine = 2;    // "어린아이의 사진을 가리키며" 부터 아이 부분이 보인다
const Rect kChildPixels{16, 8, 3, 3};       // 1장 사진 속 아이 위치
constexpr Point kPhotoOrigin{26, 3};
}  // namespace

PrologueScene::PrologueScene(GameServices& services)
    : NarrativeScene(services, {
          {"", "아버지는 몇 해 전부터 알츠하이머를 앓고 계십니다."},
          {"", "아버지는 가끔 앨범을 펼쳐 놓고 같은 사진을 오래 바라보십니다."},
          {"", "어느 날, 어린아이의 사진을 가리키며 물으셨어요."},
          {"아버지", "\"이 아이는 누구니?\""},
          {"", "흩어진 기억은 블록이 되어 쏟아집니다. 줄을 지울 때마다 사진 조각이 하나씩 돌아옵니다."},
          {"", "아버지의 가장 오래된 기억부터, 한 장씩 다시 찾아봅시다."},
      }) {}

void PrologueScene::renderBackground(Console& console) const {
    const PhotoArt& photo = m_services.catalog.get(0).photo;
    console.frame({kPhotoOrigin.x - 2, kPhotoOrigin.y - 1, 52, 18}, Color::DarkYellow);
    console.draw({kPhotoOrigin.x + 16, kPhotoOrigin.y - 1}, "[ 아버지의 앨범 ]", Color::Black, Color::DarkYellow);
    PhotoPainter::paint(console, kPhotoOrigin, photo, PhotoTone::Faded);

    if (lineIndex() >= kPointingLine) {
        const Point childOrigin{kPhotoOrigin.x + kChildPixels.x * 2, kPhotoOrigin.y + kChildPixels.y};
        PhotoPainter::paint(console, childOrigin, photo, kChildPixels, PhotoTone::Vivid);
        if (timeInLineMs() / 400 % 2 == 0) {
            console.draw({childOrigin.x + 7, childOrigin.y + 1}, "<- ?", Color::Yellow);
        }
    }
}

void PrologueScene::onNarrationEnd() {
    changeTo(std::make_unique<PlayScene>(m_services, 0));
}
