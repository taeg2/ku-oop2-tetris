#include "medicine/Medicine.h"

Medicine::Medicine(const std::string& name) : name(name) {}

Medicine::~Medicine() = default;

std::string Medicine::getName() const {
    return name;
}
