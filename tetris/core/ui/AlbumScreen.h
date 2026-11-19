#pragma once
#include "ui/PixelArt.h"
// 앨범 그림과 상태 (책, 걸린 사진, 상세 보기, 페이지 넘김). 입력 연결은 AlbumView가 한다
class AlbumScreen {
public:
    static const int SLOTS = 4, STAGES = 2, FLIP_END = 64;
    void open(int cleared, int t);
    bool isOpen() const { return open_; }
    void close() { open_ = false; }
    void left();
    void right();
    int selected() const { return sel_; }
    void draw(Frame& f, int t, const Frame* preview = nullptr) const;
    bool inDetail() const { return detail_; }
    void openDetail() { detail_ = true; focus_ = 1; }
    void closeDetail() { detail_ = false; }
    void up() { focus_ = 0; }
    void down() { focus_ = 1; }
    bool playChosen() const { return detail_ && focus_ == 1 && sel_ <= unlocked_; }
    void setStage(int s) { stage_ = s; }
    int stage() const { return stage_; }
    bool stageComplete() const { for (int s = 0; s < available(stage_); ++s) if (!done_[stage_][s]) return false; return true; }
    void startFlip(int t) { flipT_ = t; }
    bool flipping() const { return flipT_ >= 0; }
    bool flipDone(int t) const { return flipT_ >= 0 && t - flipT_ >= FLIP_END; }
    void endFlip() { flipT_ = -1; stage_ = 1; }
    int openedAt() const { return openT_; }
    static int available(int stage) { return stage == 0 ? 4 : 2; }
    bool cancelChosen() const { return detail_ && focus_ == 0; }
private:
    bool open_ = false;
    int unlocked_ = 0, sel_ = 0, newSlot_ = -1, openT_ = 0, focus_ = 1, stage_ = 0, flipT_ = -1;
    bool detail_ = false;
    void drawDetail(Frame& f, int t, const Frame* preview) const;
    bool done_[STAGES][SLOTS] = {};
    static void drawIcon(Frame& f, int slot, int cx, int cy);
};
