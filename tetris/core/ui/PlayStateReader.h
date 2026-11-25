#pragma once
#include <utility>
#include <vector>
// 라운드 그림이 테트리스 상태를 읽는 창구. 칸 값은 -1(빈칸) 또는 ToyBlocks 블록 종류(0~6)
// 게임에서는 SessionPlayState(StageSession), 앨범 미리보기에서는 EmptyPlayState를 쓴다
class PlayStateReader {
public:
    inline static const int kCols = 10;
    inline static const int kRows = 20;
    using Cells = std::vector<std::pair<int, int>>;
    virtual ~PlayStateReader() = default;
    virtual int at(int x, int y) const = 0;
    virtual int current() const = 0;
    virtual Cells pieceCells() const = 0;
    virtual Cells ghostCells() const = 0;
    virtual Cells shape(int type) const = 0;
    virtual int next() const = 0;
    virtual const std::vector<int>& clearingRows() const = 0;
    virtual bool clearing() const = 0;
    virtual bool over() const = 0;
    virtual bool hasPiece() const = 0;
};
