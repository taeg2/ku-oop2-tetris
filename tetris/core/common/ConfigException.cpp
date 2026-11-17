#include "common/ConfigException.h"

ConfigException::ConfigException(const std::string& detail) : GameException("설정 오류: " + detail) {}
