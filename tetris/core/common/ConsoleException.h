#pragma once
#include "common/GameException.h"

// 콘솔 API 호출 실패 (핸들을 못 얻음 등)
class ConsoleException : public GameException {
public:
    ConsoleException(const std::string& api, unsigned long errorCode);
};
