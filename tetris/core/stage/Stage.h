#pragma once

#include <array>
#include <memory>
#include <string>

#include "boss/Boss.h"
#include "play/EffectTarget.h"

// 스테이지 하나의 내용 (어린 시절, 진학과 취업, 결혼과 출산).
// 제목, 블록 테마, 사진 조각 4개의 설명, 앨범 문구와 이 스테이지의 보스를 가진다.
// 내용은 모두 main에서 넘겨주므로, 스테이지를 추가하거나 바꿀 때 이 클래스는 고치지 않는다.
class Stage {
public:
    // photoParts는 사진 조각 4개의 설명이다.
    Stage(int number,
          const std::string& title,
          const std::string& blockTheme,
          const std::array<std::string, 4>& photoParts,
          const std::string& albumCaption,
          std::unique_ptr<Boss> boss);

    // ===== 쿼리 =====

    int getNumber() const;
    std::string getTitle() const;

    // 사진 조각 수 (4)
    int getPhotoPartCount() const;

    // 블록 테마 설명 (예: "장난감, 크레파스, 그림책, 교과서")
    std::string getBlockTheme() const;

    // index번째(0 ~ 3) 사진 조각에 드러나는 부분. 범위를 벗어나면 빈 문자열이다.
    std::string getPhotoPart(int index) const;

    // 사진이 앨범에 들어갈 때 사진 아래에 표시하는 문구
    std::string getAlbumCaption() const;

    const Boss& getBoss() const;

    // ===== 커맨드: 이 스테이지의 보스 =====

    void appearBoss();
    void disappearBoss();
    void updateBoss(int elapsedMs, EffectTarget& target);

private:
    static constexpr int photoPartCount = 4;

    int number;
    std::string title;
    std::string blockTheme;
    std::array<std::string, photoPartCount> photoParts;
    std::string albumCaption;
    std::unique_ptr<Boss> boss;
};
