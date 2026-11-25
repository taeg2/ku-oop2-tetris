// 기억을 맞추는 테트리스 - 진입점
// 구체 클래스(터미널, 키보드, 사운드)는 여기서만 만들고, Game에는 추상 타입으로 넘긴다. (DIP)
// 실행 인자: --mute  (효과음 끄기)
#include <iostream>
#include <memory>
#include <string>
#include "flow/Game.h"
#include "ui/MuteSoundPlayer.h"
#ifdef _WIN32
#include "ui/BeepSoundPlayer.h"
#include "ui/WindowsKeyboardInput.h"
#include "ui/WindowsTerminal.h"
#else
#include "ui/AnsiTerminal.h"
#include "ui/PosixKeyboardInput.h"
#endif

int main(int argc, char* argv[]) {
    bool muted = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--mute") {
            muted = true;
        }
    }
    try {
#ifdef _WIN32
        WindowsTerminal terminal;
        WindowsKeyboardInput input;
        std::unique_ptr<ISoundPlayer> sound;
        if (muted) {
            sound = std::make_unique<MuteSoundPlayer>();
        } else {
            sound = std::make_unique<BeepSoundPlayer>();
        }
#else
        AnsiTerminal terminal;
        PosixKeyboardInput input;
        auto sound = std::make_unique<MuteSoundPlayer>();  // Mac/Linux에서는 효과음 없음
        (void)muted;
#endif
        Game game(terminal, input, *sound);
        game.run();
        return game.exitCode();
    } catch (const std::exception& e) {  // 화면 준비 단계에서 실패한 경우
        std::cerr << "게임을 시작할 수 없습니다: " << e.what() << std::endl;
        return 1;
    }
}
