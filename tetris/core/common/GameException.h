#pragma once
#include <stdexcept>
#include <string>

// 게임에서 던지는 예외의 뿌리. Game::run()이 이 타입으로 한꺼번에 받아 화면을 복구한다.
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message);
};
