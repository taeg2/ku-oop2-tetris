#pragma once
#include "ui/PixelArt.h"
using namespace std;

// 사람 그림들이 함께 쓰는 색 표 (글자 -> 색)
inline vector<pair<char, RGB>> peopleColors() {
    return {
        {'O', {42, 26, 28}},
        {'H', {110, 70, 44}}, {'h', {84, 52, 34}}, {'L', {146, 98, 62}},
        {'S', {246, 204, 164}}, {'s', {222, 168, 132}}, {'Q', {204, 152, 120}},
        {'E', {34, 26, 34}}, {'W', {255, 255, 255}}, {'P', {240, 150, 150}}, {'M', {176, 80, 84}},
        {'Y', {250, 210, 64}}, {'y', {208, 160, 44}},
        {'B', {72, 100, 170}}, {'b', {52, 74, 130}},
        {'K', {70, 44, 40}},
        {'D', {214, 110, 124}}, {'d', {172, 78, 96}},
        {'A', {246, 240, 228}}, {'a', {212, 204, 192}},
        {'R', {96, 58, 40}}, {'r', {72, 42, 30}}, {'l', {128, 84, 56}}
    };
}
namespace People {
inline const Sprite KID_HEAD_F("kid head front", {
    "....OOOOOOOO....",
    "..OOHHLLHHHHOO..",
    ".OHHLLLHHHHHHHO.",
    ".OHHHHHHHHHHHhO.",
    "OHHHHHHHHHHHHHhO",
    "OHHhHHHhHHHhHHhO",
    "OHSSHSSSSHSSSHhO",
    "OSSSSSSSSSSSSSsO",
    "OSSWESSSSSSWESsO",
    "OSSEESSSSSSEESsO",
    "OSPPSSSSSSSSPPsO",
    ".OSSSSSMMSSSSsO.",
    "..OSSSSSSSSSsO..",
    "...OOSSSSSSOO...",
    ".....OSSSSO.....",
}, peopleColors());
inline const Sprite KID_HEAD_B("kid head back", {
    "....OOOOOOOO....",
    "..OOHHHHHHHHOO..",
    ".OHHHLLHHHHHHHO.",
    ".OHHLLHHHHHHHhO.",
    "OHHHHHHHHHHHHHhO",
    "OHHHHHHhHHHHHHhO",
    "OHHHHHhHHHHHHhhO",
    "OHHHHHHHHHHHHhhO",
    "OsHHHHHHHHHHHhsO",
    "OsHHHHHHHHHHhhsO",
    ".OHHHHHHHHHHhhO.",
    ".OhHHHHHHHHhhhO.",
    "..OhhHHHHHhhhO..",
    "...OOhhhhhhOO...",
    ".....OSSSSO.....",
}, peopleColors());
inline const Sprite KID_HEAD_S("kid head side", {
    "....OOOOOO....",
    "..OOHHLLHHOO..",
    ".OHHLLHHHHHHO.",
    "OHHHHHHHHHHHhO",
    "OHhHHhHHHHHHhO",
    ".OSSSShHHHHHhO",
    ".OSSSSSSHHHHhO",
    ".OSESSSSsHHHhO",
    "OSSESSSQQsHHhO",
    ".OSSSSSsQsHhO.",
    ".OPSSSSSSsHhO.",
    "..OMSSSSSsHO..",
    "..OSSSSSsO....",
    "...OOSSsO.....",
    "....OSSO......",
}, peopleColors());
inline const Sprite KID_BODY_F("kid body front", {
    "....OOYYYYOO....",
    "..OOYYYYYYYYOO..",
    ".OYYOYYYYYYOYyO.",
    ".OYYOYYYYYYOYyO.",
    ".OSSOYYYYYYOSsO.",
    ".OSSOYYYYYYOSsO.",
    ".OSSOyyyyyyOSsO.",
    "..OOOBBBBBBOOO..",
    "....OBBbbBBO....",
}, peopleColors());
inline const Sprite KID_LEGS_F("kid legs front", {
    "....OBBOOBBO....",
    "....OSSOOSSO....",
    "....OSSOOSsO....",
    "....OSSOOSsO....",
    "....OSSOOSsO....",
    "....OSSOOSsO....",
    "....OSsOOSsO....",
    "...OKKKOOKKKO...",
    "...OOOOOOOOOO...",
}, peopleColors());
inline const Sprite KID_LEGS_F_STEP("kid legs front, left foot up", {
    "....OBBOOBBO....",
    "....OSSOOSSO....",
    "....OSSOOSsO....",
    "....OSSOOSsO....",
    "....OSSOOSsO....",
    "....OSsOOSsO....",
    "...OKKKOOSsO....",
    "....OOOOOKKKO...",
    "........OOOOO...",
}, peopleColors());
inline const Sprite KID_BODY_S("kid body side, arm down", {
    "....OYYYYO....",
    "...OYYYYYYO...",
    "...OYYYYYyO...",
    "...OYOYYYyO...",
    "...OYOYYYyO...",
    "...OSOYYYyO...",
    "...OSOyyyyO...",
    "...OOBBBBbO...",
    "...OBBBBBbO...",
}, peopleColors());
inline const Sprite KID_BODY_S_BACK("kid body side, arm swung back", {
    "....OYYYYO....",
    "...OYYYYYYO...",
    "...OYYYYYyO...",
    "...OYYYYOYO...",
    "...OYYYYOYYO..",
    "...OYYYYyOSO..",
    "...OyyyyyOSO..",
    "...OBBBBBOO...",
    "...OBBBBBbO...",
}, peopleColors());
inline const Sprite KID_BODY_S_FWD("kid body side, arm swung forward", {
    "....OYYYYO....",
    "...OYYYYYYO...",
    "...OYYYYYyO...",
    "..OYOYYYYyO...",
    ".OYYOYYYYyO...",
    ".OSOYYYYYyO...",
    ".OSOyyyyyyO...",
    "..OOBBBBBbO...",
    "...OBBBBBbO...",
}, peopleColors());
inline const Sprite KID_LEGS_S("kid legs side, standing", {
    "....OBBBBbO...",
    "....OBBBBbO...",
    ".....OSSsO....",
    ".....OSSsO....",
    ".....OSSsO....",
    ".....OSSsO....",
    ".....OSSsO....",
    "...OKKKKKO....",
    "...OOOOOOO....",
}, peopleColors());
inline const Sprite KID_LEGS_S_NEAR("kid legs side, near leg forward", {
    "....OBBBBbO...",
    "...OBBBBBBbO..",
    "...OSsOOQQO...",
    "...OSsO.OQQO..",
    "..OSsO..OQQO..",
    "..OSsO...OQQO.",
    ".OSsO....OQQO.",
    "OKKKO....OKKKO",
    "OOOO.....OOOO.",
}, peopleColors());
inline const Sprite KID_LEGS_S_FAR("kid legs side, far leg forward", {
    "....OBBBBbO...",
    "...OBBBBBBbO..",
    "...OQQOOSsO...",
    "...OQQO.OSsO..",
    "..OQQO..OSsO..",
    "..OQQO...OSsO.",
    ".OQQO....OSsO.",
    "OKKKO....OKKKO",
    "OOOO.....OOOO.",
}, peopleColors());
inline const Sprite MOM_HEAD_F("mom head front", {
    "......OOOO......",
    ".....OlRRRO.....",
    "....OORRRrOO....",
    "...ORRRllRRRO...",
    "..ORRllRRRRRRO..",
    "..ORRRRRRRRRrO..",
    "..ORSSSSSSSSrO..",
    "..ORSSSSSSSSrO..",
    "..ORSWESSWESRO..",
    "..ORSEESSEESrO..",
    "..ORPSSSSSSPrO..",
    "..OrSSSMMSSSrO..",
    "...OrSSSSSSrO...",
    "....OOSSSSOO....",
    "......OSSO......",
}, peopleColors());
inline const Sprite MOM_HEAD_B("mom head back", {
    "......OOOO......",
    ".....OlRRRO.....",
    "....OORRRrOO....",
    "...ORRRllRRRO...",
    "..ORRllRRRRRRO..",
    "..ORRRRRRRRRrO..",
    "..ORRRRRRRRRrO..",
    "..ORRRRRRRRRrO..",
    "..ORRRRRRRRrrO..",
    "..OSRRRRRRrrSO..",
    "..OrRRRRRRrrrO..",
    "...OrrRRRrrrO...",
    "....OOrrrrOO....",
    "....OOSSSSOO....",
    "......OSSO......",
}, peopleColors());
inline const Sprite MOM_HEAD_S("mom head side", {
    "........OOO...",
    "...OOOOOlRlO..",
    "..ORRRRROrRrO.",
    ".ORRllRRROOO..",
    ".ORRRRRRRRrO..",
    "OSSRRRRRRRrO..",
    "OSSSSRRRRRrO..",
    "OSESSSSsRRrO..",
    "OSESSSSsRRrO..",
    "OPSSSSSSSRrO..",
    ".OMSSSSSSRO...",
    "..OSSSSSsO....",
    "...OOSSSO.....",
    "....OSSO......",
}, peopleColors());
inline const Sprite MOM_BODY_F("mom body front", {
    "....OODDDDOO....",
    "...ODDAAAADDO...",
    "..ODDDAAAADDdO..",
    "..ODODAAAADOdO..",
    "..ODODAAAADOdO..",
    "..ODODAAAADOdO..",
    "..ODOAAAAAAOdO..",
    "..OSODAAAADOsO..",
    "..OOODAAAADOOO..",
    "...ODDAAAADdO...",
    "...ODDAAAADdO...",
    "..ODDDAAAADDdO..",
    "..ODDDAAAADDdO..",
    "..ODDDaaaaDDdO..",
    ".ODDDDDDDDDDDdO.",
    ".ODDDDDDDDDDDdO.",
    ".OdDDDDDDDDDddO.",
    "..OOOOOOOOOOOO..",
}, peopleColors());
inline const Sprite MOM_BODY_B("mom body back", {
    "....OODDDDOO....",
    "...ODDADDADDO...",
    "..ODDDADDADDdO..",
    "..ODODADDADOdO..",
    "..ODODDDDDDOdO..",
    "..ODODDDDDDOdO..",
    "..ODOAADDAAOdO..",
    "..OSODAAAADOsO..",
    "..OOODDAADDOOO..",
    "...ODDADDADdO...",
    "...ODDDDDDDdO...",
    "..ODDDDDDDDDdO..",
    "..ODDDDDDDDDdO..",
    "..ODDDDDDDDDdO..",
    ".ODDDDDDDDDDDdO.",
    ".ODDDDDDDDDDDdO.",
    ".OdDDDDDDDDDddO.",
    "..OOOOOOOOOOOO..",
}, peopleColors());
inline const Sprite MOM_LEGS_F("mom legs front", {
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOsO.....",
    ".....OSOOsO.....",
    "....OKKOOKKO....",
    "....OOOOOOOO....",
}, peopleColors());
inline const Sprite MOM_LEGS_F_STEP("mom legs front, left foot up", {
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOSO.....",
    ".....OSOOsO.....",
    "....OKKOOsO.....",
    "....OOOOOKKO....",
    ".........OOO....",
}, peopleColors());
inline const Sprite MOM_BODY_S("mom body side, arm down", {
    "...ODDDDO.....",
    "..OADDDDDO....",
    "..OAADDDDdO...",
    "..OAADODDdO...",
    "..OAADODDdO...",
    "..OAADODDdO...",
    "..OAAAOSDdO...",
    "..OAAAAOOdO...",
    "..OAAAADDdO...",
    ".OAAAADDDdO...",
    ".OAAAADDDDdO..",
    ".OaAAADDDDdO..",
    "OaaAAADDDDDdO.",
    "ODDDDDDDDDDdO.",
    "ODDDDDDDDDDdO.",
    "OdDDDDDDDDddO.",
    ".OOOOOOOOOOO..",
}, peopleColors());
inline const Sprite MOM_BODY_S_REACH("mom body side, arm out (drawn by code)", {
    "...ODDDDO.....",
    "..OADDDDDO....",
    "..OAADDDDdO...",
    "..OAADDDDdO...",
    "..OAADDDDdO...",
    "..OAADDDDdO...",
    "..OAAADDDdO...",
    "..OAAAADDdO...",
    "..OAAAADDdO...",
    ".OAAAADDDdO...",
    ".OAAAADDDDdO..",
    ".OaAAADDDDdO..",
    "OaaAAADDDDDdO.",
    "ODDDDDDDDDDdO.",
    "ODDDDDDDDDDdO.",
    "OdDDDDDDDDddO.",
    ".OOOOOOOOOOO..",
}, peopleColors());
inline const Sprite MOM_LEGS_S("mom legs side, standing", {
    ".....OSSO.....",
    ".....OSSO.....",
    ".....OSSO.....",
    ".....OSSO.....",
    ".....OSSO.....",
    ".....OSSO.....",
    ".....OSsO.....",
    "....OKKKO.....",
    "....OOOOO.....",
}, peopleColors());
inline const Sprite MOM_LEGS_S_NEAR("mom legs side, near leg forward", {
    ".....OSQO.....",
    ".....OSQO.....",
    ".....OSQO.....",
    "....OSSOQQO...",
    "...OSSO.OQQO..",
    "..OSSO...OQO..",
    ".OSsO.....OQO.",
    "OKKKO.....OKKO",
    "OOOO......OOOO",
}, peopleColors());
inline const Sprite MOM_LEGS_S_FAR("mom legs side, far leg forward", {
    ".....OQSO.....",
    ".....OQSO.....",
    ".....OQSO.....",
    "....OQQOSSO...",
    "...OQQO.OSSO..",
    "..OQQO...OSO..",
    ".OQQO.....OsO.",
    "OKKKO.....OKKO",
    "OOOO......OOOO",
}, peopleColors());
enum Facing { FRONT, BACK, LEFT, RIGHT };
inline void drawKid(Frame& f, int x, int y, Facing face, int step = -1, bool talk = false, bool blink = false) {
    int bob = (step >= 0 && step % 2 == 1) ? -1 : 0;
    if (face == FRONT || face == BACK) {
        int ox = x - 8;
        bool stepping = step >= 0 && step % 2 == 0;
        bool mirror = (step / 2) % 2 == 1;
        blit(f, stepping ? KID_LEGS_F_STEP : KID_LEGS_F, ox, y - 8, 1, stepping && mirror);
        blit(f, KID_BODY_F, ox, y - 17 + bob);
        blit(f, face == FRONT ? KID_HEAD_F : KID_HEAD_B, ox, y - 32 + bob);
        if (face == FRONT) {
            uint8_t eye = C(34, 26, 34), skin = C(246, 204, 164), mouth = C(176, 80, 84);
            if (blink) { for (int ex : { 3, 11 }) { f.hline(ox + ex, ox + ex + 1, y - 32 + bob + 8, skin); f.hline(ox + ex, ox + ex + 1, y - 32 + bob + 9, eye); } }
            if (talk) f.rect(ox + 7, y - 32 + bob + 12, 2, 1, mouth);
        }
        return;
    }
    bool flip = face == RIGHT;
    int ox = x - 7;
    const Sprite* legs = &KID_LEGS_S; const Sprite* body = &KID_BODY_S;
    if (step >= 0 && step % 4 == 0) { legs = &KID_LEGS_S_NEAR; body = &KID_BODY_S_BACK; }
    if (step >= 0 && step % 4 == 2) { legs = &KID_LEGS_S_FAR; body = &KID_BODY_S_FWD; }
    blit(f, *legs, ox, y - 8, 1, flip);
    blit(f, *body, ox, y - 17 + bob, 1, flip);
    blit(f, KID_HEAD_S, ox, y - 32 + bob, 1, flip);
    if (talk) f.set(flip ? ox + 11 : ox + 2, y - 32 + bob + 13, C(176, 80, 84));
}
inline void drawMom(Frame& f, int x, int y, Facing face, int step = -1, bool talk = false, bool blink = false, bool reach = false) {
    int bob = (step >= 0 && step % 2 == 1) ? -1 : 0;
    if (face == FRONT || face == BACK) {
        int ox = x - 8;
        bool stepping = step >= 0 && step % 2 == 0;
        bool mirror = (step / 2) % 2 == 1;
        blit(f, stepping ? MOM_LEGS_F_STEP : MOM_LEGS_F, ox, y - 8, 1, stepping && mirror);
        blit(f, face == FRONT ? MOM_BODY_F : MOM_BODY_B, ox, y - 26 + bob);
        blit(f, face == FRONT ? MOM_HEAD_F : MOM_HEAD_B, ox, y - 40 + bob);
        if (face == FRONT) {
            uint8_t eye = C(34, 26, 34), skin = C(246, 204, 164), mouth = C(176, 80, 84);
            if (blink) for (int ex : { 5, 9 }) { f.hline(ox + ex, ox + ex + 1, y - 40 + bob + 8, skin); f.hline(ox + ex, ox + ex + 1, y - 40 + bob + 9, eye); }
            if (talk) f.rect(ox + 7, y - 40 + bob + 12, 2, 1, mouth);
        }
        return;
    }
    bool flip = face == RIGHT;
    int ox = x - 7;
    const Sprite* legs = &MOM_LEGS_S;
    if (step >= 0 && step % 4 == 0) legs = &MOM_LEGS_S_NEAR;
    if (step >= 0 && step % 4 == 2) legs = &MOM_LEGS_S_FAR;
    blit(f, *legs, ox, y - 8, 1, flip);
    blit(f, reach ? MOM_BODY_S_REACH : MOM_BODY_S, ox, y - 25 + bob, 1, flip);
    blit(f, MOM_HEAD_S, ox, y - 38 + bob, 1, flip);
    if (talk) f.set(flip ? ox + 11 : ox + 2, y - 38 + bob + 11, C(176, 80, 84));
}
}
