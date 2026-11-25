#pragma once

// 게임 수치는 모두 여기에 모은다. 기획안 9장의 "미정 사항"은 이 값을 바꿔 가며 조정한다.
namespace config {

// 화면
const int kScreenWidth = 100;
const int kScreenHeight = 30;
const int kFrameMs = 16;
const int kMaxFrameDeltaMs = 100;

// 사진 진행 (기획안 3장)
const int kLinesPerPiece = 3;
const int kPhotoPieces = 4;
const int kBossAppearPieces = 2;
const int kStageCount = 3;

// 낙하 속도: 간격 = 시작 간격 x kSpeedUpFactor^단계 (조각 하나마다 1단계)
const double kSpeedUpFactor = 0.8;
const int kMinDropIntervalMs = 90;
const int kStage1DropMs = 800;
const int kStage2DropMs = 680;
const int kStage3DropMs = 560;
const int kLineFlashMs = 280;

// 약 (기획안 6장)
const int kPillBlockChancePercent = 12;  // 약이 붙은 블록이 나올 확률
const int kPillLimitPerType = 2;         // 종류별 보유 한도
const double kBluePillSlowFactor = 2.0;  // 낙하 간격 x2
const int kBluePillDurationMs = 8000;
const bool kRedPillCountsAsLine = true;  // 빨간 약으로 지운 줄도 사진 조각 줄 수에 포함

// 보스 (기획안 5장)
const int kBlackoutIntervalMs = 9000;
const int kBlackoutDurationMs = 4000;
const int kBlackoutWidth = 5;
const int kBlackoutHeight = 9;
const int kShapeShiftIntervalMs = 6000;
const int kShapeShiftWarningMs = 1500;
const int kForgetIntervalMs = 9000;
const int kForgetWarningMs = 2500;
const int kForgetCellCount = 3;

// 연출 시간
const int kStageIntroMs = 2200;
const int kStageCompleteMs = 2200;
const int kStageFailMs = 1400;
const int kTypingCharsPerSecond = 28;
const int kFinalAlbumMs = 5000;  // 마지막 앨범을 보여 준 뒤 엔딩으로 넘어가기까지

}  // namespace config

// 플레이 화면 배치 (콘솔 좌표)
namespace layout {
// ui의 그림 View(집, 앨범, 라운드, 메뉴, 엔딩)를 그리는 위치. 그림은 Sixel 이미지 하나로 화면 전체를 덮는다.
const int kArtX = 0;
const int kArtY = 0;
const int kBoardX = 1;
const int kBoardY = 2;
const int kNextX = 27;
const int kNextY = 2;
const int kHudX = 27;
const int kHudY = 9;
const int kPhotoX = 45;
const int kPhotoY = 2;
const int kMessageX = 45;
const int kMessageY = 22;
const int kMessageWidth = 53;
const int kMessageHeight = 5;
const int kHelpY = 28;
}  // namespace layout
