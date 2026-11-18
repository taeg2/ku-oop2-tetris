#pragma once
#include "common/Point.h"
#include "ui/PixelImage.h"

namespace WindowsSixelBlockRenderer {
void draw(Point cellOrigin, const PixelImage& image, int scale);
}
