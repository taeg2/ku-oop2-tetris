#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct Rgb {
    std::uint8_t r = 0, g = 0, b = 0;
};

class PixelImage {
public:
    PixelImage(int width, int height, Rgb background);

    static PixelImage fromKeyMap(int width, int height, const std::vector<std::string>& rows,
                                  const std::map<char, Rgb>& palette, Rgb background);

    int width() const { return m_width; }
    int height() const { return m_height; }
    Rgb background() const { return m_background; }
    Rgb at(int x, int y) const { return m_pixels[static_cast<std::size_t>(y * m_width + x)]; }
    bool isBackground(int x, int y) const;

private:
    int m_width;
    int m_height;
    Rgb m_background;
    std::vector<Rgb> m_pixels;
};
