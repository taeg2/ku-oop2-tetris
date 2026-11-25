#pragma once

// 게임 수치는 모두 여기에 모은다. 기획안 9장의 "미정 사항"은 이 값을 바꿔 가며 조정한다.
namespace config {

// 화면
constexpr int kScreenWidth = 100;
constexpr int kScreenHeight = 30;
constexpr int kFrameMs = 16;
constexpr int kMaxFrameDeltaMs = 100;

// 사진 진행 (기획안 3장)
constexpr int kLinesPerPiece = 3;
constexpr int kPhotoPieces = 4;
constexpr int kBossAppearPieces = 2;
constexpr int kStageCount = 3;

// 낙하 속도: 간격 = 시작 간격 x kSpeedUpFactor^단계 (조각 하나마다 1단계)
constexpr double kSpeedUpFactor = 0.8;
constexpr int kMinDropIntervalMs = 90;
constexpr int kStage1DropMs = 800;
constexpr int kStage2DropMs = 680;
constexpr int kStage3DropMs = 560;
constexpr int kLineFlashMs = 280;

// 약 (기획안 6장)
constexpr int kPillBlockChancePercent = 12;  // 약이 붙은 블록이 나올 확률
constexpr int kPillLimitPerType = 2;         // 종류별 보유 한도
constexpr double kBluePillSlowFactor = 2.0;  // 낙하 간격 x2
constexpr int kBluePillDurationMs = 8000;
constexpr bool kRedPillCountsAsLine = true;  // 빨간 약으로 지운 줄도 사진 조각 줄 수에 포함

// 보스 (기획안 5장)
constexpr int kBlackoutIntervalMs = 9000;
constexpr int kBlackoutDurationMs = 4000;
constexpr int kBlackoutWidth = 5;
constexpr int kBlackoutHeight = 9;
constexpr int kShapeShiftIntervalMs = 6000;
constexpr int kShapeShiftWarningMs = 1500;
constexpr int kForgetIntervalMs = 9000;
constexpr int kForgetWarningMs = 2500;
constexpr int kForgetCellCount = 3;

// 연출 시간
constexpr int kStageIntroMs = 2200;
constexpr int kStageCompleteMs = 2200;
constexpr int kStageFailMs = 1400;
constexpr int kTypingCharsPerSecond = 28;
constexpr int kFinalAlbumMs = 5000;  // 마지막 앨범을 보여 준 뒤 엔딩으로 넘어가기까지

}  // namespace config

// 플레이 화면 배치 (콘솔 좌표)
namespace layout {
// ui의 그림 View(집, 앨범, 라운드, 메뉴, 엔딩)를 그리는 위치. 그림은 Sixel 이미지 하나로 화면 전체를 덮는다.
constexpr int kArtX = 0;
constexpr int kArtY = 0;
constexpr int kBoardX = 1;
constexpr int kBoardY = 2;
constexpr int kNextX = 27;
constexpr int kNextY = 2;
constexpr int kHudX = 27;
constexpr int kHudY = 9;
constexpr int kPhotoX = 45;
constexpr int kPhotoY = 2;
constexpr int kMessageX = 45;
constexpr int kMessageY = 22;
constexpr int kMessageWidth = 53;
constexpr int kMessageHeight = 5;
constexpr int kHelpY = 28;
}  // namespace layout
