#include "medicine/MedicineInventory.h"

#include <utility>

MedicineInventory::MedicineInventory(int capacity)
    : medicines(), capacity(capacity) {}

MedicineInventory::MedicineInventory(const MedicineInventory& other)
    : medicines(), capacity(other.capacity) {
    copyMedicinesFrom(other);
}

int MedicineInventory::getCount() const {
    return static_cast<int>(medicines.size());
}

int MedicineInventory::getCapacity() const {
    return capacity;
}

bool MedicineInventory::isFull() const {
    return getCount() >= capacity;
}

std::string MedicineInventory::getName(int index) const {
    if (!isValidIndex(index)) {
        return "";
    }
    return medicines[index]->getName();
}

void MedicineInventory::add(std::unique_ptr<Medicine> medicine) {
    if (medicine == nullptr || isFull()) {
        return;
    }
    medicines.push_back(std::move(medicine));
}

void MedicineInventory::use(int index, EffectTarget& target) {
    if (!isValidIndex(index)) {
        return;
    }
    // 목록에서 먼저 빼고 효과를 건다
    std::unique_ptr<Medicine> medicine = std::move(medicines[index]);
    medicines.erase(medicines.begin() + index);
    medicine->applyTo(target);
}

void MedicineInventory::clear() {
    medicines.clear();
}

void MedicineInventory::restoreFrom(const MedicineInventory& other) {
    if (&other == this) {
        return;
    }
    capacity = other.capacity;
    medicines.clear();
    copyMedicinesFrom(other);
}

bool MedicineInventory::isValidIndex(int index) const {
    return index >= 0 && index < getCount();
}

void MedicineInventory::copyMedicinesFrom(const MedicineInventory& other) {
    for (const auto& medicine : other.medicines) {
        medicines.push_back(medicine->clone());
    }
}
