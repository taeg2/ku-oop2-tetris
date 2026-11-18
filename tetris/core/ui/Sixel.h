#pragma once
#include <string>
#include "ui/PixelImage.h"

namespace Sixel {
std::string encode(const PixelImage& image, int scale);
}
