#include "common/ConfigException.h"
using namespace std;

ConfigException::ConfigException(const string& detail) : GameException("설정 오류: " + detail) {}
