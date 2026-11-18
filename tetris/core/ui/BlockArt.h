#pragma once
#include "block/TetrominoType.h"
#include "ui/PixelImage.h"

namespace BlockArt {
constexpr int kSize = 8;
constexpr int kScale = 3;

PixelImage tile(int stageIndex, TetrominoType type);
}
