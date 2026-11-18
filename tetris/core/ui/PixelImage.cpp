#include "ui/PixelImage.h"
#include "common/ConfigException.h"

PixelImage::PixelImage(int width, int height, Rgb background)
    : m_width(width), m_height(height), m_background(background),
      m_pixels(static_cast<std::size_t>(width * height), background) {}

bool PixelImage::isBackground(int x, int y) const {
    const Rgb c = at(x, y);
    return c.r == m_background.r && c.g == m_background.g && c.b == m_background.b;
}

PixelImage PixelImage::fromKeyMap(int width, int height, const std::vector<std::string>& rows,
                                  const std::map<char, Rgb>& palette, Rgb background) {
    if (static_cast<int>(rows.size()) != height) {
        throw ConfigException("PixelImage height is not " + std::to_string(height));
    }
    PixelImage image(width, height, background);
    for (int y = 0; y < height; ++y) {
        const std::string& row = rows[static_cast<std::size_t>(y)];
        if (static_cast<int>(row.size()) != width) {
            throw ConfigException("PixelImage width is not " + std::to_string(width));
        }
        for (int x = 0; x < width; ++x) {
            const char key = row[static_cast<std::size_t>(x)];
            if (key == '.') {
                continue;
            }
            const auto it = palette.find(key);
            if (it == palette.end()) {
                throw ConfigException(std::string("unknown block art color character '") + key + "'");
            }
            image.m_pixels[static_cast<std::size_t>(y * width + x)] = it->second;
        }
    }
    return image;
}
