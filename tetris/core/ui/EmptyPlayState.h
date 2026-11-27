#pragma once
#include "ui/PlayStateReader.h"
#include "ui/SessionPlayState.h"
using namespace std;

// 빈 보드. 앨범에서 라운드 배경만 미리 보여 줄 때 쓴다
class EmptyPlayState : public PlayStateReader {
public:
    int at(int, int) const override { return -1; }
    int current() const override { return 0; }
    Cells pieceCells() const override { return {}; }
    Cells ghostCells() const override { return {}; }
    Cells shape(int type) const override { return SessionPlayState::shapeOf(type); }
    int next() const override { return 2; }
    const vector<int>& clearingRows() const override { return m_rows; }
    bool clearing() const override { return false; }
    bool over() const override { return false; }
    bool hasPiece() const override { return false; }
    int pillAt(int, int) const override { return -1; }
    int piecePill() const override { return -1; }
    int piecePillIndex() const override { return -1; }
    int pillCount(int) const override { return 0; }
    bool hidden(int, int) const override { return false; }
    bool marked(int, int) const override { return false; }
private:
    vector<int> m_rows;
};
