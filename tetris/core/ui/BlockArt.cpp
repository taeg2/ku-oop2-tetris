#include "ui/BlockArt.h"
#include <map>
#include <vector>
#include "common/ConfigException.h"

namespace {

const std::vector<std::string>& bevelTemplate() {
    static const std::vector<std::string> kTemplate = {
        "HHHHHHHH",
        "HBBBBBLL",
        "HBBBBBLL",
        "HBBBBBLL",
        "HBBBBBLL",
        "HBBBBBLL",
        "HLLLLLLL",
        "HLLLLLLL",
    };
    return kTemplate;
}

struct Shade {
    Rgb hi, base, lo;
};

Shade shadeFor(int stage, TetrominoType type) {
    switch (stage) {
        case 0:
            switch (type) {
                case TetrominoType::I: return {{137, 246, 255}, {92, 201, 235}, {37, 146, 180}};
                case TetrominoType::O: return {{255, 254, 118}, {231, 209, 73}, {176, 154, 18}};
                case TetrominoType::T: return {{212, 138, 243}, {167, 93, 198}, {112, 38, 143}};
                case TetrominoType::J: return {{124, 147, 255}, {79, 102, 216}, {24, 47, 161}};
                case TetrominoType::L: return {{255, 193, 104}, {237, 148, 59}, {182, 93, 4}};
                case TetrominoType::S: return {{159, 240, 137}, {114, 195, 92}, {59, 140, 37}};
                case TetrominoType::Z: return {{255, 126, 126}, {230, 81, 81}, {175, 26, 26}};
            }
            break;
        case 1:
            switch (type) {
                case TetrominoType::I: return {{165, 195, 235}, {120, 150, 190}, {65, 95, 135}};
                case TetrominoType::O: return {{255, 250, 205}, {222, 205, 160}, {167, 150, 105}};
                case TetrominoType::T: return {{140, 140, 153}, {95, 95, 108}, {40, 40, 53}};
                case TetrominoType::J: return {{108, 117, 177}, {63, 72, 132}, {8, 17, 77}};
                case TetrominoType::L: return {{242, 203, 115}, {197, 158, 70}, {142, 103, 15}};
                case TetrominoType::S: return {{153, 173, 137}, {108, 128, 92}, {53, 73, 37}};
                case TetrominoType::Z: return {{195, 107, 107}, {150, 62, 62}, {95, 7, 7}};
            }
            break;
        case 2:
            switch (type) {
                case TetrominoType::I: return {{203, 255, 255}, {158, 210, 236}, {103, 155, 181}};
                case TetrominoType::O: return {{255, 255, 253}, {248, 233, 208}, {193, 178, 153}};
                case TetrominoType::T: return {{245, 215, 255}, {200, 170, 222}, {145, 115, 167}};
                case TetrominoType::J: return {{197, 205, 255}, {152, 160, 224}, {97, 105, 169}};
                case TetrominoType::L: return {{255, 240, 185}, {228, 195, 140}, {173, 140, 85}};
                case TetrominoType::S: return {{217, 255, 227}, {172, 216, 182}, {117, 161, 127}};
                case TetrominoType::Z: return {{255, 205, 215}, {234, 160, 170}, {179, 105, 115}};
            }
            break;
        default:
            break;
    }
    throw ConfigException("unknown block art stage (" + std::to_string(stage) + ")");
}

constexpr Rgb kBackground{254, 0, 253};

}

namespace BlockArt {

PixelImage tile(int stageIndex, TetrominoType type) {
    const Shade shade = shadeFor(stageIndex, type);
    const std::map<char, Rgb> palette = {{'H', shade.hi}, {'B', shade.base}, {'L', shade.lo}};
    return PixelImage::fromKeyMap(kSize, kSize, bevelTemplate(), palette, kBackground);
}

}
