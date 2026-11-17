#pragma once

#include <memory>
#include <random>
#include <vector>

#include "block/PieceGenerator.h"

// "가방" 방식으로 블록을 고르는 생성기.
// 원형 블록을 한 개씩 가방에 넣고 섞은 뒤 하나씩 꺼낸다. 가방이 비면 다시 채운다.
// 그래서 원형 블록 수(7종이면 7개)만큼 꺼낼 때마다 모든 종류가 한 번씩 나온다.
//
// 원형 블록은 main에서 만들어 넘긴다. 이 클래스는 구체 블록 클래스를 모른다. (DIP)
class BagPieceGenerator : public PieceGenerator {
public:
    // prototypes: 나올 수 있는 블록의 원형. 1개 이상이어야 한다.
    // seed: 섞을 때 쓰는 난수 시드. 같은 시드면 같은 순서로 나온다.
    BagPieceGenerator(std::vector<std::unique_ptr<Tetromino>> prototypes,
                      unsigned int seed = std::random_device()());

    const Tetromino& peekNext() const override;
    void advance() override;

private:
    void refillBag();

    std::vector<std::unique_ptr<Tetromino>> prototypes;
    std::vector<int> bag;   // 아직 꺼내지 않은 원형 블록 번호. 맨 뒤가 다음 블록이다.
    std::mt19937 random;
};
