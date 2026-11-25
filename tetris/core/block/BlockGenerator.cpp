#include "block/BlockGenerator.h"
#include <algorithm>
#include <array>

namespace {
const Point kSpawnPosition{3, -1};
}

BlockGenerator::BlockGenerator(std::mt19937& rng, int pillChancePercent)
    : m_rng(rng), m_pillChance(pillChancePercent) {
    refillBag();
}

void BlockGenerator::refillBag() {
    std::array<TetrominoType, kTetrominoTypeCount> bag{};
    for (int i = 0; i < kTetrominoTypeCount; ++i) {
        bag[i] = static_cast<TetrominoType>(i);
    }
    std::shuffle(bag.begin(), bag.end(), m_rng);
    for (TetrominoType type : bag) {
        m_queue.push_back(makePiece(type));
    }
}

Tetromino BlockGenerator::makePiece(TetrominoType type) {
    Tetromino piece(type, kSpawnPosition);
    std::uniform_int_distribution<int> percent(0, 99);
    if (percent(m_rng) < m_pillChance) {
        std::uniform_int_distribution<int> cell(0, Tetromino::kCellCount - 1);
        const PillType pill = percent(m_rng) < 50 ? PillType::Blue : PillType::Red;
        piece = piece.withPill(pill, cell(m_rng));
    }
    return piece;
}

const Tetromino& BlockGenerator::nextPiece() const {
    return m_queue.front();
}

TetrominoType BlockGenerator::previewType() const {
    return m_queue.at(1).type();
}

void BlockGenerator::advance() {
    m_queue.pop_front();
    if (m_queue.size() < 2) {
        refillBag();  // 항상 2개 이상 남겨 둔다 (nextPiece + previewType)
    }
}

void BlockGenerator::reset() {
    m_queue.clear();
    refillBag();
}
