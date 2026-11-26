#include "common/ConsoleException.h"
using namespace std;

ConsoleException::ConsoleException(const string& api, unsigned long errorCode)
    : GameException("콘솔 오류: " + api + " 실패 (코드 " + to_string(errorCode) + ")") {}
