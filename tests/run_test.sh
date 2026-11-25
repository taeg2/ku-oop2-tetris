#!/bin/bash
# 게임 로직 자동 검증: 봇이 3개 스테이지를 끝까지 플레이하고, 주요 장면 화면을 test_screens/에 저장한다.
# 사용법 (Mac/Linux):  bash tests/run_test.sh
set -e
cd "$(dirname "$0")"
SRC=../tetris/core
FILES=$(find $SRC -name '*.cpp' ! -name main.cpp)
mkdir -p test_screens
${CXX:-c++} -std=c++17 -O1 -I$SRC $FILES BotPlayTest.cpp -o bot_play_test -lpthread
./bot_play_test
