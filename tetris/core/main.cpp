// 기억을 맞추는 테트리스
//
// 구체 클래스를 만드는 코드는 모두 이 파일에 모은다. (컨벤션 5장 DIP)
// 다른 클래스는 추상 클래스(PieceGenerator, Boss, Medicine, Renderer, InputSource, Scene)에만 의존한다.
// 기획안에서 아직 정해지지 않은 수치도 여기 한 곳에서 바꾼다.

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "block/BagPieceGenerator.h"
#include "block/IBlock.h"
#include "block/JBlock.h"
#include "block/LBlock.h"
#include "block/OBlock.h"
#include "block/SBlock.h"
#include "block/TBlock.h"
#include "block/ZBlock.h"
#include "boss/DarknessBoss.h"
#include "boss/ForgetBoss.h"
#include "boss/ShapeShiftBoss.h"
#include "flow/AlbumScene.h"
#include "flow/Game.h"
#include "flow/PlayScene.h"
#include "flow/StoryScene.h"
#include "medicine/BluePill.h"
#include "medicine/MedicineInventory.h"
#include "medicine/RedPill.h"
#include "play/FallSpeed.h"
#include "play/PlayField.h"
#include "stage/Album.h"
#include "stage/Stage.h"
#include "stage/StageSession.h"
#include "ui/ConsoleIO.h"

namespace {
    // ===== 게임 설정 (기획안 9장 미정 사항은 테스트하며 조정) =====

    const int boardWidth = 10;
    const int boardHeight = 20;

    const int linesPerPhotoPiece = 3;     // 사진 조각 하나에 필요한 줄 수

    const int startFallIntervalMs = 800;  // 스테이지 시작 낙하 간격
    const int fallIntervalStepMs = 100;   // 조각마다 줄어드는 낙하 간격
    const int minFallIntervalMs = 150;
    const int slowMultiplier = 2;         // 파란 약을 먹었을 때 낙하 간격 배수

    const int medicineCapacity = 2;
    const int bluePillDurationMs = 5000;

    // ===== 만들기 =====

    std::vector<std::unique_ptr<Tetromino>> makeBlockPrototypes() {
        std::vector<std::unique_ptr<Tetromino>> prototypes;
        prototypes.push_back(std::make_unique<IBlock>());
        prototypes.push_back(std::make_unique<OBlock>());
        prototypes.push_back(std::make_unique<TBlock>());
        prototypes.push_back(std::make_unique<SBlock>());
        prototypes.push_back(std::make_unique<ZBlock>());
        prototypes.push_back(std::make_unique<JBlock>());
        prototypes.push_back(std::make_unique<LBlock>());
        return prototypes;
    }

    // 사진 조각을 채울 때마다 차례로 주는 약
    std::vector<std::unique_ptr<Medicine>> makeRewardMedicines() {
        std::vector<std::unique_ptr<Medicine>> medicines;
        medicines.push_back(std::make_unique<BluePill>("파란 약", bluePillDurationMs));
        medicines.push_back(std::make_unique<RedPill>("빨간 약"));
        return medicines;
    }

    // 보스 이름은 기획안 미정이라 가칭이다.
    std::vector<std::unique_ptr<Stage>> makeStages() {
        std::vector<std::unique_ptr<Stage>> stages;

        stages.push_back(std::make_unique<Stage>(
            1, "어린 시절", "장난감, 크레파스, 그림책, 교과서",
            std::array<std::string, 4>{ "어린 주인공의 얼굴", "아이를 안고 있는 어머니", "옆에 선 아버지", "배경 (집 앞 마당)" },
            "엄마 품이 좋아서, 사진을 찍는 내내 내려오지 않으려 했다.",
            std::make_unique<DarknessBoss>("깜빡임 (가칭)", 8000, 4, 3000)));

        stages.push_back(std::make_unique<Stage>(
            2, "진학과 취업", "교과서, 참고서, 학사모, 이력서, 넥타이",
            std::array<std::string, 4>{ "학사모를 쓴 주인공", "꽃다발을 든 어머니", "어깨에 손을 올린 아버지", "배경 (학교 정문)" },
            "첫 출근 날, 집을 나서기 전에 사원증을 세 번이나 확인했다.",
            std::make_unique<ShapeShiftBoss>("뒤바뀜 (가칭)", 7000)));

        stages.push_back(std::make_unique<Stage>(
            3, "결혼과 출산", "꽃다발, 반지, 턱시도, 집 열쇠, 아기 신발",
            std::array<std::string, 4>{ "주인공", "배우자", "두 사람 품에 안긴 아기", "배경 (함께 사는 집)" },
            "처음 너를 안았을 때, 너무 작아서 어떻게 안아야 할지 몰랐다.",
            std::make_unique<ForgetBoss>("잊혀짐 (가칭)", 10000, 3)));

        return stages;
    }

    std::vector<std::string> makePrologue() {
        return {
            "기억을 맞추는 테트리스",
            "",
            "\"아버지는 가끔 앨범을 펼쳐 놓고 같은 사진을 오래 바라보십니다.\"",
            "\"어느 날, 어린아이의 사진을 가리키며 물으셨어요. '이 아이는 누구니?'\"",
            "\"아버지의 가장 오래된 기억부터, 한 장씩 다시 찾아봅시다.\"",
            "",
            "줄을 " + std::to_string(linesPerPhotoPiece) + "줄 지울 때마다 가족사진 조각이 하나씩 채워집니다.",
        };
    }

    std::vector<std::string> makeStageIntro(const Stage& stage) {
        return {
            "STAGE " + std::to_string(stage.getNumber()) + "  " + stage.getTitle(),
            "",
            stage.getBlockTheme() + " 사이로 흐릿한 기억이 떠오릅니다.",
            "사진이 절반쯤 채워지면 기억을 방해하는 무언가가 나타납니다.",
        };
    }

    std::vector<std::string> makeEnding() {
        return {
            "현재의 주인공이 완성된 앨범을 펼칩니다.",
            "",
            "자녀: \"이날 기억나요, 아버지?\"",
            "주인공: \"응. 네가 너무 작아서, 어떻게 안아야 할지 몰랐지.\"",
            "",
            "주인공이 자녀의 이름을 부릅니다.",
            "",
            "- 끝 -",
        };
    }
}

int main() {
    // 블록과 한 판 진행
    BagPieceGenerator generator(makeBlockPrototypes());
    PlayField field(boardWidth, boardHeight, generator,
                    FallSpeed(startFallIntervalMs, fallIntervalStepMs, minFallIntervalMs, slowMultiplier));

    // 스테이지 3개와 스테이지마다의 진행 규칙
    std::vector<std::unique_ptr<Stage>> stages = makeStages();
    MedicineInventory medicines(medicineCapacity);
    std::vector<std::unique_ptr<StageSession>> sessions;
    for (auto& stage : stages) {
        sessions.push_back(std::make_unique<StageSession>(
            *stage, field, medicines, linesPerPhotoPiece, makeRewardMedicines()));
    }
    Album album;

    // 장면 순서: 프롤로그 → (스테이지 소개 → 플레이 → 앨범) x 3 → 엔딩
    std::vector<std::unique_ptr<Scene>> scenes;
    scenes.push_back(std::make_unique<StoryScene>(makePrologue()));
    for (size_t i = 0; i < stages.size(); ++i) {
        scenes.push_back(std::make_unique<StoryScene>(makeStageIntro(*stages[i])));
        scenes.push_back(std::make_unique<PlayScene>(*sessions[i], field));
        scenes.push_back(std::make_unique<AlbumScene>(album, *stages[i]));
    }
    scenes.push_back(std::make_unique<StoryScene>(makeEnding()));

    // 콘솔 화면·입력으로 실행 (그래픽으로 바꾸려면 Renderer·InputSource를 구현한 클래스만 바꾸면 된다)
    ConsoleIO console;
    Game game(console, console, std::move(scenes));
    game.run();
    return 0;
}
