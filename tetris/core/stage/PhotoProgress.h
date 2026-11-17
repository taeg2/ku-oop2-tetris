#pragma once

// 한 스테이지의 가족사진(4조각)이 얼마나 채워졌는지 관리한다.
// 지운 줄 수를 누적해서, linesPerPiece줄을 지울 때마다 사진 조각이 하나씩 채워진다.
// 여러 줄을 한 번에 지워도 모두 누적한다.
//
// 조각이 채워질 때 속도를 올리거나 보스를 부르는 일은 StageSession이 이 클래스를 보고 정한다.
class PhotoProgress {
public:
    // linesPerPiece: 사진 조각 하나를 채우는 데 필요한 줄 수 (기획안 가정값 3, 1 이상)
    explicit PhotoProgress(int linesPerPiece = 3);

    // ===== 쿼리 =====

    // 채워진 사진 조각 수 (0 ~ getPieceCount())
    int getFilledPieceCount() const;

    // 사진 한 장의 전체 조각 수 (4)
    int getPieceCount() const;

    // 이 스테이지에서 지금까지 지운 줄 수
    int getClearedLines() const;

    // 다음 조각을 채우기까지 더 지워야 하는 줄 수. 사진이 완성되었으면 0이다.
    int getLinesUntilNextPiece() const;

    // 사진이 절반(2/4) 이상 채워졌는지. 보스가 등장하는 조건이다.
    bool isHalfFilled() const;

    // 사진이 모두(4/4) 채워졌는지. 스테이지 클리어 조건이다.
    bool isComplete() const;

    // ===== 커맨드 =====

    // count줄을 지웠음을 누적한다.
    void addClearedLines(int count);

    // 빈 사진 틀(0/4)로 되돌린다. (스테이지 시작·재도전)
    void reset();

private:
    static constexpr int pieceCount = 4;

    int linesPerPiece;
    int clearedLines;
};
