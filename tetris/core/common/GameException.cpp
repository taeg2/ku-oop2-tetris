#include "common/GameException.h"
using namespace std;

GameException::GameException(const string& message) : runtime_error(message) {}
