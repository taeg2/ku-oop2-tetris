#pragma once

#include <map>
#include <string>
#include <vector>

#ifndef _WIN32
#include <termios.h>
#endif

#include "block/Tetromino.h"
#include "boss/Boss.h"
#include "stage/PhotoProgress.h"
#include "ui/InputSource.h"
#include "ui/Renderer.h"

// 콘솔 화면 출력과 키 입력을 담당한다.
// Windows와 Mac에서 다른 코드(키 입력, 콘솔 설정)는 모두 이 클래스 안에만 둔다. (컨벤션 8장)
// - 화면: 두 OS 모두 ANSI 이스케이프 코드(커서 이동, 색)로 그린다.
// - 입력: Windows는 <conio.h>, Mac은 <termios.h>로 키를 기다리지 않고 읽는다.
//
// 화면은 한 프레임을 문자열에 모았다가 endFrame()에서 한 번에 출력해서 깜빡임을 줄인다.
class ConsoleIO : public Renderer, public InputSource {
public:
    ConsoleIO();
    ~ConsoleIO() override;

    // 콘솔 설정을 한 객체만 바꾸고 되돌리도록 복사를 막는다.
    ConsoleIO(const ConsoleIO& other) = delete;
    ConsoleIO& operator=(const ConsoleIO& other) = delete;

    // ===== InputSource =====

    std::vector<Action> getActions() const override;
    void poll() override;

    // ===== Renderer =====

    void beginFrame() override;
    void drawPlay(const PlayField& field, const StageSession& session) override;
    void drawStory(const std::vector<std::string>& lines) override;
    void drawAlbum(const Album& album, int newPhotoIndex) override;
    void endFrame() override;

private:
    // ===== 화면 문자열 만들기 (쿼리) =====

    std::vector<std::string> makeBoardLines(const PlayField& field, const Boss& boss) const;
    std::vector<std::string> makePanelLines(const PlayField& field, const StageSession& session) const;
    std::vector<std::string> makePhotoFrameLines(const PhotoProgress& progress) const;
    std::vector<std::string> makeNextPieceLines(const Tetromino& piece) const;

    // 블록 기호 하나를 색칠한 칸(가로 2글자)으로 만든다.
    std::string makeCell(char symbol) const;

    // 가운데 정렬한 줄
    std::string makeCentered(const std::string& text) const;

    // 화면에서 차지하는 칸 수 (한글은 2칸)
    int getDisplayWidth(const std::string& text) const;

    // 밀리초를 "1.5초" 형태로
    std::string formatSeconds(int ms) const;

    // ===== 커맨드 =====

    void addLine(const std::string& line);

    // 키 하나를 조작으로 바꿔 actions에 넣는다. 정해지지 않은 키는 무시한다.
    void addKey(int key);

    // 기다리지 않고, 지금까지 들어온 키를 모두 읽는다. (OS마다 다름)
    void readPendingKeys();

    // 콘솔을 게임용으로 설정하고(UTF-8, 색, 커서 숨김, 즉시 입력), 끝날 때 되돌린다. (OS마다 다름)
    void setUpConsole();
    void restoreConsole();

    std::string frame;
    std::vector<Action> actions;
    std::map<char, std::string> blockColors;
    std::map<int, Action> keyActions;

#ifdef _WIN32
    unsigned long originalOutputMode;
#else
    termios originalTerminal;
#endif
};
