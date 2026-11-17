#include "ui/PhotoPainter.h"

void PhotoPainter::paint(Console& console, Point origin, const PhotoArt& photo) {
    paint(console, origin, photo, PhotoTone::Vivid);
}

void PhotoPainter::paint(Console& console, Point origin, const PhotoArt& photo, PhotoTone tone) {
    paint(console, origin, photo, Rect{0, 0, PhotoArt::kWidth, PhotoArt::kHeight}, tone);
}

void PhotoPainter::paint(Console& console, Point origin, const PhotoArt& photo, const Rect& pixelArea, PhotoTone tone) {
    for (int y = pixelArea.y; y < pixelArea.bottom(); ++y) {
        for (int x = pixelArea.x; x < pixelArea.right(); ++x) {
            const Color color = toned(photo.pixel(x, y), tone);
            const Point pos{origin.x + widthInColumns(x - pixelArea.x), origin.y + (y - pixelArea.y)};
            console.fill({pos.x, pos.y, 2, 1}, color);
        }
    }
}

void PhotoPainter::paintThumbnail(Console& console, Point origin, const PhotoArt& photo) {
    for (int y = 0; y < PhotoArt::kHeight / 2; ++y) {
        for (int x = 0; x < PhotoArt::kWidth / 2; ++x) {
            const Color color = photo.pixel(x * 2, y * 2 + 1);
            console.fill({origin.x + x * 2, origin.y + y, 2, 1}, color);
        }
    }
}

Color PhotoPainter::toned(Color color, PhotoTone tone) {
    if (tone == PhotoTone::Vivid || color == Color::Black) {
        return color;
    }
    const bool bright = color == Color::White || color == Color::Yellow || color == Color::Cyan ||
                        color == Color::Gray || color == Color::Green || color == Color::Red;
    if (tone == PhotoTone::Faded) {
        return bright ? Color::Gray : Color::DarkGray;
    }
    return bright ? Color::DarkGray : Color::Black;  // Hidden
}
