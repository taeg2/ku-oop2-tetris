#pragma once
#include <array>
#include <vector>
#include "common/Point.h"
#include "flow/NarrativeScene.h"

class PhotoArt;

// 엔딩 (결말). 앨범을 펼쳐 세 장을 차례로 보고, 마지막 사진이 테트로미노 조각으로
//     흩어졌다가 다시 맞춰진 뒤, 아버지가 자녀를 알아본다. (기획안 7장 최종 이벤트)
class EndingScene : public NarrativeScene {
public:
    explicit EndingScene(GameServices& services);

protected:
    void renderBackground(Console& console) const override;
    void onNarrationEnd() override;
    void onLineShown(std::size_t index) override;
    void onUpdate(int dtMs) override;
    bool canAdvance() const override;

private:
    struct Fragment {
        std::vector<Point> blocks;  // 4x4 픽셀 블록 좌표 (테트로미노 모양)
        Point scatter;              // 흩어진 위치 (콘솔 칸 단위 오프셋)
    };

    void renderAlbumSpread(Console& console) const;
    void renderReassembly(Console& console) const;
    void renderFrame(Console& console, const std::string& caption) const;
    const PhotoArt& photo(int index) const;

    std::vector<Fragment> m_fragments;
    int m_assembleMs = -1;  // -1: 아직 시작 안 함
};
