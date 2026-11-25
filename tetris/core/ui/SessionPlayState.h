#pragma once
#include "play/StageSession.h"
#include "ui/PlayStateReader.h"
using namespace std;

// StageSession을 읽기만 해서 PlayStateReader로 보여 주는 어댑터. 블록 색을 그림용 블록 종류로 바꾼다
class SessionPlayState : public PlayStateReader {
public:
    explicit SessionPlayState(const StageSession& session);
    int at(int x, int y) const override;
    int current() const override;
    Cells pieceCells() const override;
    Cells ghostCells() const override;
    Cells shape(int type) const override;
    int next() const override;
    const vector<int>& clearingRows() const override;
    bool clearing() const override;
    bool over() const override;
    bool hasPiece() const override;
    static int toArtType(TetrominoType type);
    static Cells shapeOf(int type);
    static TetrominoType fromArtType(int type);
private:
    static Cells toCells(const Tetromino::Cells& cells);
    const StageSession& m_session;
};
