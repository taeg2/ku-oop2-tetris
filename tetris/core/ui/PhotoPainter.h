#pragma once
#include "stage/PhotoArt.h"
#include "common/Rect.h"
#include "ui/Console.h"

// 사진을 어떤 색감으로 그릴지
enum class PhotoTone {
    Vivid,   // 원래 색 (되찾은 기억)
    Faded,   // 회색 (희미한 기억)
    Hidden   // 거의 안 보임 (아직 못 찾은 기억)
};

// PhotoArt를 콘솔에 그리는 도구. 픽셀 1개 = 콘솔 2칸 x 1줄.
class PhotoPainter {
public:
    static void paint(Console& console, Point origin, const PhotoArt& photo);
    static void paint(Console& console, Point origin, const PhotoArt& photo, PhotoTone tone);
    static void paint(Console& console, Point origin, const PhotoArt& photo, const Rect& pixelArea, PhotoTone tone);
    static void paintThumbnail(Console& console, Point origin, const PhotoArt& photo);  // 가로세로 절반 크기

    static constexpr int widthInColumns(int pixels) { return pixels * 2; }

private:
    static Color toned(Color color, PhotoTone tone);
};
