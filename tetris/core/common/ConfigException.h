#pragma once
#include "common/GameException.h"

// 스테이지·사진 데이터가 잘못됨 (사진 크기, 알 수 없는 색 문자 등)
class ConfigException : public GameException {
public:
    explicit ConfigException(const std::string& detail);
};
