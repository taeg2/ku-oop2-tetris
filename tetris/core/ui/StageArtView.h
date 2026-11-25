#pragma once
#include <memory>
#include <string>
#include <vector>
#include "play/IGameEventListener.h"
#include "play/StageSession.h"
#include "ui/RoundScene.h"
#include "ui/SessionPlayState.h"
#include "ui/SixelEncoder.h"
#include "ui/View.h"
using namespace std;

// Stage 1 플레이 화면 (호수, 부엌, 해변, 침실). EventBus에 구독해야 줄 삭제와 사진 완성을 받는다
// 장면은 wantsPause()가 true인 동안 블록 진행을 멈추고, 라운드가 끝나면 앨범을 연다
class StageArtView : public View, public IGameEventListener {
public:
    inline static const int kScale = 2;
    inline static const int kFrameMs = 100;
    StageArtView(Point origin, const StageSession& session, int linesPerRound);  // linesPerRound: 라운드 하나를 끝내는 실제 줄 수
    void update(int dtMs) override;
    void draw(Console& console) const override;
    void onEvent(const GameEvent& event) override;
    bool wantsPause() const;  // 줄 삭제 연출이나 라운드 끝 연출 중이면 true
    int frameCount() const { return m_t; }
    const Frame& frame() const { return m_frame; }
    void render() const;
private:
    RoundScene& round() const { return *m_rounds[static_cast<size_t>(m_round)]; }
    SessionPlayState m_state;
    vector<unique_ptr<RoundScene>> m_rounds;
    int m_round = 0;
    int m_lines = 0, m_visual = 0, m_linesPerRound = 3;
    int m_t = 0;
    int m_accMs = 0;
    int m_flashT = -1;
    bool m_advance = false;
    mutable Frame m_frame;  // draw()는 const라서 그림 캐시는 mutable로 둔다
    mutable int m_drawnT = -1;
    mutable string m_sixel;
    SixelEncoder m_encoder;
};
