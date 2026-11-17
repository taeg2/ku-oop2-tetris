#include "common/GameException.h"

GameException::GameException(const std::string& message) : std::runtime_error(message) {}
