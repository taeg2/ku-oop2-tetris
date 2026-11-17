#pragma once

#include <memory>
#include <vector>

#include "medicine/Medicine.h"
#include "medicine/MedicineInventory.h"
#include "play/PlayField.h"
#include "play/PlayListener.h"
#include "stage/PhotoProgress.h"
#include "stage/Stage.h"

// 스테이지 하나를 플레이하는 동안의 규칙을 담당한다. (기획안 3장)
// PlayField가 알려 주는 줄 삭제를 받아서
// - 사진 조각을 채우고, 조각마다 낙하 속도를 올리고, 약을 하나 준다.
// - 사진이 절반(2/4) 채워지면 보스를 부르고, 완성(4/4)되면 보스를 보내고 클리어한다.
// 실패한 뒤 restart()를 부르면 보드·사진 조각·속도·보스·약을 스테이지 시작 상태로 되돌린다.
class StageSession : public PlayListener {
public:
    // linesPerPiece: 사진 조각 하나에 필요한 줄 수 (기획안 가정값 3)
    // rewardPrototypes: 사진 조각을 채울 때마다 차례로 하나씩 주는 약의 원형.
    //                   비어 있으면 약을 주지 않는다. (약 획득 방식은 기획안 미정)
    StageSession(Stage& stage,
                 PlayField& field,
                 MedicineInventory& medicines,
                 int linesPerPiece,
                 std::vector<std::unique_ptr<Medicine>> rewardPrototypes);

    // ===== 쿼리 =====

    const Stage& getStage() const;
    const PhotoProgress& getProgress() const;
    const MedicineInventory& getMedicines() const;

    // 사진을 완성해 스테이지를 클리어했는지
    bool isCleared() const;

    // 새 블록을 놓을 공간이 없어 실패했는지
    bool isFailed() const;

    // ===== 커맨드 =====

    // 스테이지를 시작한다. 지금 가진 약을 "스테이지 시작 상태"로 기억하고,
    // PlayField의 알림을 이 객체가 받도록 연결한다.
    void start();

    // 스테이지 시작 상태로 되돌린다. (실패 후 재도전)
    void restart();

    // 시간이 elapsedMs만큼 흘렀음을 알린다. 블록 낙하와 보스 기믹이 진행된다.
    // 클리어했거나 실패했으면 아무 일도 하지 않는다.
    void update(int elapsedMs);

    // 가진 약 중 index번 칸의 약을 쓴다. 플레이 중일 때만 쓸 수 있다.
    void useMedicine(int index);

    // ===== PlayField 알림 (PlayListener) =====

    void onPieceLocked() override;
    void onLinesCleared(int count) override;
    void onGameOver() override;

private:
    bool isPlaying() const;

    // 사진 조각 하나가 새로 채워졌을 때의 처리 (속도 증가, 약 지급)
    void onPhotoPieceFilled();

    Stage& stage;
    PlayField& field;
    MedicineInventory& medicines;
    MedicineInventory medicinesAtStart;
    PhotoProgress progress;
    std::vector<std::unique_ptr<Medicine>> rewardPrototypes;
    int nextRewardIndex;
    bool cleared;
};
