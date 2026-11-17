#include "common/TextUtil.h"

std::u32string TextUtil::decode(const std::string& utf8) {
    std::u32string out;
    out.reserve(utf8.size());
    for (std::size_t i = 0; i < utf8.size();) {
        const auto c = static_cast<unsigned char>(utf8[i]);
        char32_t cp = 0;
        int extra = 0;
        if (c < 0x80) { cp = c; }
        else if ((c >> 5) == 0x6) { cp = c & 0x1F; extra = 1; }
        else if ((c >> 4) == 0xE) { cp = c & 0x0F; extra = 2; }
        else if ((c >> 3) == 0x1E) { cp = c & 0x07; extra = 3; }
        else { ++i; continue; }  // 깨진 바이트는 건너뛴다
        if (i + static_cast<std::size_t>(extra) >= utf8.size()) {
            break;  // 끝이 잘린 문자
        }
        for (int k = 1; k <= extra; ++k) {
            cp = (cp << 6) | (static_cast<unsigned char>(utf8[i + k]) & 0x3F);
        }
        out.push_back(cp);
        i += static_cast<std::size_t>(extra) + 1;
    }
    return out;
}

std::string TextUtil::encode(const std::u32string& text) {
    std::string out;
    for (char32_t cp : text) {
        if (cp < 0x80) {
            out += static_cast<char>(cp);
        } else if (cp < 0x800) {
            out += static_cast<char>(0xC0 | (cp >> 6));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            out += static_cast<char>(0xE0 | (cp >> 12));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            out += static_cast<char>(0xF0 | (cp >> 18));
            out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }
    return out;
}

int TextUtil::charWidth(char32_t ch) {
    const bool wide = (ch >= 0x1100 && ch <= 0x115F) || (ch >= 0x2E80 && ch <= 0xA4CF) ||
                      (ch >= 0xAC00 && ch <= 0xD7A3) || (ch >= 0xF900 && ch <= 0xFAFF) ||
                      (ch >= 0xFE30 && ch <= 0xFE4F) || (ch >= 0xFF00 && ch <= 0xFF60) ||
                      (ch >= 0xFFE0 && ch <= 0xFFE6);
    return wide ? 2 : 1;
}

int TextUtil::width(const std::u32string& text) {
    int total = 0;
    for (char32_t ch : text) {
        total += charWidth(ch);
    }
    return total;
}

int TextUtil::width(const std::string& utf8) {
    return width(decode(utf8));
}

std::vector<std::u32string> TextUtil::wrap(const std::u32string& text, int maxWidth) {
    std::vector<std::u32string> lines;
    std::u32string line;
    int lineWidth = 0;
    std::size_t lastSpace = std::u32string::npos;

    for (char32_t ch : text) {
        if (ch == U'\n') {
            lines.push_back(line);
            line.clear();
            lineWidth = 0;
            lastSpace = std::u32string::npos;
            continue;
        }
        const int w = charWidth(ch);
        if (lineWidth + w > maxWidth && !line.empty()) {
            if (lastSpace != std::u32string::npos && ch != U' ') {
                std::u32string rest = line.substr(lastSpace + 1);
                line.erase(lastSpace);
                lines.push_back(line);
                line = rest;
            } else {
                lines.push_back(line);
                line.clear();
            }
            lineWidth = width(line);
            lastSpace = std::u32string::npos;
            if (ch == U' ') {
                continue;
            }
        }
        if (ch == U' ') {
            lastSpace = line.size();
        }
        line.push_back(ch);
        lineWidth += w;
    }
    if (!line.empty()) {
        lines.push_back(line);
    }
    return lines;
}
