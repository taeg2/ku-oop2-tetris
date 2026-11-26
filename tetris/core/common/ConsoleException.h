#pragma once
#include "common/GameException.h"
using namespace std;

// 콘솔 API 호출 실패 (핸들을 못 얻음 등)
class ConsoleException : public GameException {
public:
    ConsoleException(const string& api, unsigned long errorCode);
};
