#pragma once
#include <deque>
#include <random>
#include "block/Tetromino.h"
using namespace std;

// 다음 블록을 만든다. 7종을 한 봉지에 넣고 섞는 7-bag 방식이라 같은 모양이 오래 안 나오는 일이 없다.
//     일정 확률로 블록 한 칸에 약을 붙인다. (기획안 6장 획득 방식 ①)
class BlockGenerator {
public:
    BlockGenerator(mt19937& rng, int pillChancePercent);

    const Tetromino& nextPiece() const;  // 다음에 나올 블록
    TetrominoType previewType() const;   // 그다음 블록 (미리보기 상자용)
    void advance();                      // 다음 블록을 꺼낸다
    void reset();

private:
    void refillBag();
    Tetromino makePiece(TetrominoType type);

    mt19937& m_rng;
    int m_pillChance;
    deque<Tetromino> m_queue;
};
