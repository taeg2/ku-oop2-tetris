#include "stage/StageCatalog.h"
#include <memory>
#include "common/ConfigException.h"
#include "boss/BlackoutGimmick.h"
#include "boss/ForgetGimmick.h"
#include "common/GameConfig.h"
#include "boss/ShapeShiftGimmick.h"

namespace {

PhotoArt childhoodPhoto() {
    return PhotoArt("어린 시절 가족사진", {
        "cccccccccccccccccccccccc",
        "ccccccccccccwwcccccccssc",
        "ccRRRcccccccwwwwcccccssc",
        "RRRRRRRccccccccccccccccc",
        "RRRRRRRRcckkkcccyyyccccc",
        "RRRRRRRRRkkkkkcyyyyycccc",
        "wwwwwwwwckskskcysysycccc",
        "wwwwwwwwccsssccysssycccc",
        "wccwyywwccbbbccykkkycccc",
        "wccwyywwcbbbbbcrsssrcccc",
        "wwwwyywwcsbbbscswwwscccc",
        "wwwwyywwggbbbgggrrrggggg",
        "ggggggggggBgBgggRgRggggg",
        "GGGGGGGGGGBGBGGGRGRGGGGG",
        "GGGGGGGGGGkGkGGGyGyGGGGG",
        "GGGGGGGGGGGGGGGGGGGGGGGG",
    }, {"빨간 지붕 집 앞마당이 떠오른다.", "아이를 안고 웃는 젊은 어머니.",
        "옆에 선 아버지의 든든한 어깨.", "어머니 품에 안긴 아이... 그게 나였다. 앨범 첫 장 완성."},
    "엄마 품이 좋아서, 사진을 찍는 내내 내려오지 않으려 했다.");
}

PhotoArt graduationPhoto() {
    return PhotoArt("졸업식 가족사진", {
        "cccccccccccccccccccccccc",
        "cccccccccccccccccccccccc",
        "AAAAAAAAAAAAAAAAAAAAAAAA",
        "aaaaaaaaakkkkkkkaaaaaaaa",
        "accaaccaackkkkkyaccaacca",
        "accyyycaackkkkkyacAAAcca",
        "aayyyyyaaakskskaaAAAAAaa",
        "acysysyaacyssscaaAsAsAca",
        "acysssrvacykkkcaacssscca",
        "aayvvvvraakkkkkaaaBBBaaa",
        "aavvvvGGaaskkksaaBBBBBaa",
        "AAsvvvsAAAAkkkAAAsBBBsAA",
        "aaavvvaaaaakakaaaaBBBaaa",
        "aaaBaBaaaaakakaaaakakaaa",
        "aaaBaBaaaaakakaaaakakaaa",
        "aaayayaaaaaaaaaaaaAaAaaa",
    }, {"꽃다발을 든 어머니.", "학사모를 쓴 내가 서 있다.",
        "학교 정문 앞, 그날의 햇살.", "어깨를 토닥여 주던 아버지. 앨범 두 번째 장 완성."},
    "졸업식 날, 어머니는 내 학사모를 몇 번이고 다시 씌워 주셨다.");
}

PhotoArt newbornPhoto() {
    return PhotoArt("아이가 태어난 날", {
        "RRRRRRRRRRRRRRRRRRRRRRRR",
        "yyyyyyyyyyyyyyyyyyyyyyyy",
        "yywwwwwwyyyyyyyyyyyRRRRR",
        "yywcwccwkkkyyyRRRyyRgggR",
        "yywwwwwkkkkkyRRRRRyRgggR",
        "yywcwcckskskyRsRsRyRRRRR",
        "yywwwwwwsssyyRsssRyyyyyy",
        "yyyyyyyybbbyyRvvvRyyyyyy",
        "yyyyyyybbbbwkkwvvvyyyyyy",
        "yyyyyyysbbbwsswvvsyyyyyy",
        "yyyyyyyybbbwwwwvvyyyyyyy",
        "yyyyyyyyByByyywywyyyyyyy",
        "RRRRRRRRBRBRRRwRwRRRRRRR",
        "rrrrrrrrkrkrrrRrRrrrrrrr",
        "rrrrrrrrrrrrrrrrrrrrrrrr",
        "rrrrrrrrrrrrrrrrrrrrrrrr",
    }, {"창문으로 들어오던 오후 햇빛.", "벽에 걸린 우리 집 첫 그림.",
        "나와 아내, 그리고...", "두 사람 품에 안긴 아기. 앨범 세 번째 장 완성."},
    "처음 너를 안았을 때, 너무 작아서 어떻게 안아야 할지 몰랐다.");
}

}  // namespace

StageCatalog::StageCatalog() {
    m_stages.push_back(StageConfig{
        "STAGE 1", "어린 시절", "장난감, 크레파스, 그림책", childhoodPhoto(), config::kStage1DropMs,
        [] {
            return Boss("짙은 안개", "단기기억상실",
                        "방금 쌓은 기억이 안개 속으로 사라진다...",
                        std::make_unique<BlackoutGimmick>(config::kBlackoutIntervalMs, config::kBlackoutDurationMs,
                                                          config::kBlackoutWidth, config::kBlackoutHeight));
        }});
    m_stages.push_back(StageConfig{
        "STAGE 2", "진학과 취업", "교과서, 학사모, 이력서, 넥타이", graduationPhoto(), config::kStage2DropMs,
        [] {
            return Boss("엉킨 실타래", "착각",
                        "분명 이 모양이었는데... 기억이 뒤엉킨다.",
                        std::make_unique<ShapeShiftGimmick>(config::kShapeShiftIntervalMs, config::kShapeShiftWarningMs));
        }});
    m_stages.push_back(StageConfig{
        "STAGE 3", "결혼과 출산", "꽃다발, 반지, 집 열쇠, 아기 신발", newbornPhoto(), config::kStage3DropMs,
        [] {
            return Boss("지우개", "까먹음",
                        "소중한 순간이 하나씩 지워지려 한다.",
                        std::make_unique<ForgetGimmick>(config::kForgetIntervalMs, config::kForgetWarningMs,
                                                        config::kForgetCellCount));
        }});
}

const StageConfig& StageCatalog::get(int index) const {
    if (index < 0 || index >= count()) {
        throw ConfigException("스테이지 번호 " + std::to_string(index) + " 가 범위를 벗어났습니다");
    }
    return m_stages[static_cast<std::size_t>(index)];
}
