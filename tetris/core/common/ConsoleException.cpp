#include "common/ConsoleException.h"

ConsoleException::ConsoleException(const std::string& api, unsigned long errorCode)
    : GameException("콘솔 오류: " + api + " 실패 (코드 " + std::to_string(errorCode) + ")") {}
