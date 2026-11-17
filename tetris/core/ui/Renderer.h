#pragma once

#include <string>
#include <vector>

#include "play/PlayField.h"
#include "stage/Album.h"
#include "stage/StageSession.h"

// 화면 출력의 추상 클래스.
// 게임 쪽(장면)은 "무엇을 그릴지"만 이 클래스에 알리고, "어떻게 그릴지"는 자식 클래스가 정한다.
// 지금은 콘솔(ConsoleIO)로 그리지만, 그래픽 화면은 이 클래스를 상속한 새 클래스를 추가하면 되고
// 게임 코드는 고치지 않는다. (OCP, DIP)
//
// 한 화면(프레임)은 beginFrame() → draw...() → endFrame() 순서로 그린다.
class Renderer {
public:
    virtual ~Renderer();

    // 새 화면을 그리기 시작한다.
    virtual void beginFrame() = 0;

    // 플레이 화면: 왼쪽에 보드, 오른쪽에 사진 틀·진행도·속도·다음 블록·약·보스 정보를 그린다.
    // 클리어하거나 실패했으면 그에 맞는 안내도 함께 그린다.
    virtual void drawPlay(const PlayField& field, const StageSession& session) = 0;

    // 자막 화면: 프롤로그·엔딩처럼 문장 여러 줄을 보여 준다.
    virtual void drawStory(const std::vector<std::string>& lines) = 0;

    // 앨범 화면: 지금까지 모은 사진을 보여 준다. newPhotoIndex번 사진은 새로 들어온 사진으로 강조한다.
    virtual void drawAlbum(const Album& album, int newPhotoIndex) = 0;

    // 그린 화면을 실제로 내보낸다.
    virtual void endFrame() = 0;
};
