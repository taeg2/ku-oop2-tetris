cmake_minimum_required(VERSION 3.20)

project(tetris LANGUAGES CXX)

# C++20 표준만 사용한다 (컴파일러 전용 확장 기능은 끈다)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# tetris/core 폴더와 그 하위 폴더의 .cpp, .h 파일을 자동으로 모두 포함한다
# 새 파일이나 새 폴더를 추가해도 이 파일은 고치지 않아도 된다
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tetris/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/tetris/core/*.h"
)

add_executable(core ${CORE_SOURCES})

# #include "block/Tetromino.h" 처럼 tetris/core 기준 경로로 헤더를 찾게 한다
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/tetris/core")

if(MSVC)
    # /utf-8: 소스 파일의 한글이 깨지지 않게 한다
    target_compile_options(core PRIVATE /W3 /utf-8)
else()
    target_compile_options(core PRIVATE -Wall -Wextra)
endif()
