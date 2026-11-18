#pragma once
#include <string>
#include <vector>

// UTF-8 문자열 도우미. 한글은 콘솔에서 2칸, ASCII는 1칸을 차지하므로
//        줄바꿈과 가운데 정렬은 바이트 수가 아니라 화면 폭으로 계산해야 한다.
class TextUtil {
public:
    static std::u32string decode(const std::string& utf8);
    static std::string encode(const std::u32string& text);
    static int charWidth(char32_t ch);
    static int width(const std::string& utf8);
    static int width(const std::u32string& text);
    static std::vector<std::u32string> wrap(const std::u32string& text, int maxWidth);
};
