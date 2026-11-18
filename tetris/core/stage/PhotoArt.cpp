#include "stage/PhotoArt.h"
#include <stdexcept>
#include "common/ConfigException.h"

PhotoArt::PhotoArt(std::string title, std::vector<std::string> pixels,
                   std::array<std::string, kPieceCount> pieceCaptions, std::string albumCaption)
    : m_title(std::move(title)), m_pixels(std::move(pixels)),
      m_pieceCaptions(std::move(pieceCaptions)), m_albumCaption(std::move(albumCaption)) {
    if (m_pixels.size() != static_cast<std::size_t>(kHeight)) {
        throw ConfigException("사진 '" + m_title + "' 높이가 " + std::to_string(kHeight) + "이 아닙니다");
    }
    for (const std::string& row : m_pixels) {
        if (row.size() != static_cast<std::size_t>(kWidth)) {
            throw ConfigException("사진 '" + m_title + "' 너비가 " + std::to_string(kWidth) + "이 아닙니다");
        }
        for (char key : row) {
            colorOf(key);  // 모르는 색 문자면 여기서 예외
        }
    }
}

Color PhotoArt::pixel(int x, int y) const {
    return colorOf(m_pixels.at(static_cast<std::size_t>(y)).at(static_cast<std::size_t>(x)));
}

Color PhotoArt::pixel(Point p) const {
    return pixel(p.x, p.y);
}

const std::string& PhotoArt::pieceCaption(int index) const {
    return m_pieceCaptions.at(static_cast<std::size_t>(index));
}

Rect PhotoArt::quadrant(int index) {
    if (index < 0 || index >= kPieceCount) {
        throw std::out_of_range("PhotoArt::quadrant " + std::to_string(index));
    }
    const int halfW = kWidth / 2;
    const int halfH = kHeight / 2;
    return Rect{(index % 2) * halfW, (index / 2) * halfH, halfW, halfH};
}

Color PhotoArt::colorOf(char key) {
    switch (key) {
        case 'k': return Color::Black;
        case 's': return Color::Yellow;  // 피부
        case 'w': return Color::White;
        case 'r': return Color::Red;
        case 'R': return Color::DarkRed;
        case 'b': return Color::Blue;
        case 'B': return Color::DarkBlue;
        case 'g': return Color::Green;
        case 'G': return Color::DarkGreen;
        case 'y': return Color::DarkYellow;
        case 'v': return Color::DarkMagenta;
        case 'c': return Color::Cyan;
        case 'C': return Color::DarkCyan;
        case 'a': return Color::Gray;
        case 'A': return Color::DarkGray;
        default: throw ConfigException(std::string("알 수 없는 사진 색 문자 '") + key + "'");
    }
}
