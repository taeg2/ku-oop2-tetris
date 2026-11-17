#pragma once

#include <memory>
#include <string>
#include <vector>

#include "medicine/Medicine.h"
#include "play/EffectTarget.h"

// 플레이어가 가진 약 목록. 정해진 개수(capacity)까지만 가질 수 있다.
// 약은 칸 번호(0부터)로 골라 쓴다. 쓴 약은 목록에서 빠지고 뒤의 약이 앞으로 당겨진다.
//
// 복사 생성자로 만든 복사본은 약도 clone()으로 모두 복사된다.
// 스테이지를 시작할 때 복사본을 만들어 두었다가, 재도전할 때 restoreFrom()으로 되돌린다.
class MedicineInventory {
public:
    // capacity: 최대 보유 개수 (기획안 미정이라 main에서 넘겨준다)
    explicit MedicineInventory(int capacity = 2);
    MedicineInventory(const MedicineInventory& other);

    // 대입은 값을 돌려주는 커맨드가 되어 CQS에 맞지 않으므로 막고, restoreFrom()을 쓴다.
    MedicineInventory& operator=(const MedicineInventory& other) = delete;

    // ===== 쿼리 =====

    int getCount() const;
    int getCapacity() const;
    bool isFull() const;

    // index번 칸의 약 이름. 칸이 비었으면 빈 문자열이다.
    std::string getName(int index) const;

    // ===== 커맨드 =====

    // 약을 넣는다. 이미 가득 찼으면 넣지 않는다.
    void add(std::unique_ptr<Medicine> medicine);

    // index번 칸의 약을 target에 쓰고 목록에서 뺀다. 칸이 비었으면 아무 일도 하지 않는다.
    void use(int index, EffectTarget& target);

    // 약을 모두 버린다.
    void clear();

    // other와 같은 상태(최대 개수, 약 목록)로 되돌린다. 약은 clone()으로 복사한다.
    void restoreFrom(const MedicineInventory& other);

private:
    bool isValidIndex(int index) const;
    void copyMedicinesFrom(const MedicineInventory& other);

    std::vector<std::unique_ptr<Medicine>> medicines;
    int capacity;
};
