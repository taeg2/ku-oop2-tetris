#pragma once
#include "ui/PixelArt.h"
#include "ui/Command.h"
#include "ui/HouseRooms.h"
#include "ui/People.h"
#include <string>
#include <vector>
using namespace std;

// 집 안을 걸어 다니는 오프닝의 흐름과 그림. 방 배경은 HouseRooms가 그린다
class HouseScene {
public:
    void start(int stage, int t);
    bool active() const { return phase_ != DONE && phase_ != OFF; }
    bool finished() const { return phase_ == DONE; }
    void handle(Command k, int t);
    void update(int t);
    void draw(Frame& out, int t) const;
    float kidX() const { return kx_; }
    float kidY() const { return ky_; }
    int room() const { return room_; }
    bool inDialog() const { return !lines_.empty(); }
    bool freeToWalk() const { return (phase_ == FREE || phase_ == ALBUM_HUNT) && lines_.empty(); }
    bool hasCandy() const { return phase_ >= WOBBLE; }
    int stage() const { return stage_; }
private:
    // (첫 판) 노인이 약을 먹고 잠듦 -> 아이로 깨어남 -> 대화 -> 딸이 나감 -> 부엌에서 음식 -> (사탕이면 비틀거림) -> 서재 앨범 -> 페이드
    enum Phase { OFF, OLD_SIT, OLD_DRINK, OLD_SLEEPY, OLD_SLEEP, WAKE, TALK, MOM_LEAVE, HUNGRY, FREE, CANDY, WOBBLE, ALBUM_HUNT, ALBUM, FADE, DONE };
    class Line { public: string who, text; };
    Phase phase_ = OFF;
    HouseRooms::Room room_ = HouseRooms::LIVING;
    float kx_ = 152, ky_ = 112;
    People::Facing face_ = People::LEFT;
    int walkT_ = -10, step_ = 0, phaseT_ = 0, roomT_ = -100;
    float momX_ = 84, momY_ = 112;
    int doorOpenT_ = -1;
    bool momGone_ = false;
    int libraryT_ = -1, animT_ = -100, candyStep_ = 0, albumStep_ = 0, stage_ = 0;
    void drawHero(Frame& world, int x, int y, People::Facing face, int step, bool talk, bool blink) const;
    void drawOpening(Frame& world, int t) const;
    mutable bool oldLook_ = false;
    bool nearFood() const;
    vector<Line> lines_;
    int lineT_ = 0;
    string note_, noteSub_;
    void say(const vector<Line>& lines, int t);
    bool talking() const { return !lines_.empty(); }
    void advance(int t);
    void setPhase(Phase p, int t) { phase_ = p; phaseT_ = t; }
    bool blocked(float x, float y) const;
    void move(float dx, float dy, int t);
    void enter(HouseRooms::Room r, float x, float y, People::Facing f, int t);
    bool nearCandy() const;
    bool nearAlbum() const;
};
