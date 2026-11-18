#ifdef _WIN32
#include "ui/WindowsSixelBlockRenderer.h"
#include "ui/Sixel.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace WindowsSixelBlockRenderer {

void draw(Point cellOrigin, const PixelImage& image, int scale) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out == INVALID_HANDLE_VALUE || out == nullptr) {
        return;
    }
    SetConsoleCursorPosition(out, COORD{static_cast<SHORT>(cellOrigin.x), static_cast<SHORT>(cellOrigin.y)});
    const std::string sequence = Sixel::encode(image, scale);
    DWORD written = 0;
    WriteFile(out, sequence.data(), static_cast<DWORD>(sequence.size()), &written, nullptr);
}

}
#endif
