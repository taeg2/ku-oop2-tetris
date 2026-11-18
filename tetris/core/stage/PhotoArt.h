#pragma once
#include <array>
#include <string>
#include <vector>
#include "common/Color.h"
#include "common/Point.h"
#include "common/Rect.h"

// 가족사진 한 장 (24 x 16 픽셀 도트). 글자 한 개가 픽셀 한 개의 색을 뜻한다.
//     4분할 조각마다 "무엇이 드러나는지" 설명과 앨범 문구도 함께 가진다. (기획안 4, 7장)
class PhotoArt {
public:
    static constexpr int kWidth = 24;
    static constexpr int kHeight = 16;
    static constexpr int kPieceCount = 4;

    PhotoArt(std::string title, std::vector<std::string> pixels,
             std::array<std::string, kPieceCount> pieceCaptions, std::string albumCaption);

    Color pixel(int x, int y) const;
    Color pixel(Point p) const;
    const std::string& title() const { return m_title; }
    const std::string& pieceCaption(int index) const;
    const std::string& albumCaption() const { return m_albumCaption; }

    static Rect quadrant(int index);  // 조각 index(0~3)의 픽셀 영역: 왼위, 오위, 왼아래, 오아래

private:
    static Color colorOf(char key);

    std::string m_title;
    std::vector<std::string> m_pixels;
    std::array<std::string, kPieceCount> m_pieceCaptions;
    std::string m_albumCaption;
};
