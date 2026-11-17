#include "block/BagPieceGenerator.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

BagPieceGenerator::BagPieceGenerator(std::vector<std::unique_ptr<Tetromino>> prototypes,
                                     unsigned int seed)
    : prototypes(std::move(prototypes)), bag(), random(seed) {
    if (this->prototypes.empty()) {
        throw std::invalid_argument("BagPieceGenerator: 원형 블록이 1개 이상 필요하다");
    }
    refillBag();
}

const Tetromino& BagPieceGenerator::peekNext() const {
    return *prototypes[bag.back()];
}

void BagPieceGenerator::advance() {
    bag.pop_back();
    if (bag.empty()) {
        refillBag();
    }
}

void BagPieceGenerator::refillBag() {
    bag.clear();
    for (int i = 0; i < static_cast<int>(prototypes.size()); ++i) {
        bag.push_back(i);
    }
    std::shuffle(bag.begin(), bag.end(), random);
}
