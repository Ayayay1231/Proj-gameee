#ifndef WEAPON_H
#define WEAPON_H
#include <string>

using namespace std;

class Weapon {
public:
    string name;
    int level;
    int baseBonusDmg;
    int bonusCrit;

    Weapon() : level(1), baseBonusDmg(0), bonusCrit(0), name("Unknown") {}
    virtual ~Weapon() {}

    // คำนวณดาเมจจากเลเวล: ดาเมจพื้นฐาน + (เลเวล * 5)
    virtual int getTotalBonusDmg() {
        return baseBonusDmg + (level * 5);
    }

    void upgrade() {
        level++;
    }
};

// --- ส่วนที่เพิ่มเข้ามาเพื่อจัดการในไฟล์เดียว ---
#include "gun.h"
#include "baseball.h"
#include "bow.h"

class WeaponFactory {
public:
    static Weapon* selectWeapon(int choice) {
        switch (choice) {
            case 1:  return new Gun();
            case 2:  return new BaseballBat();
            case 3:  return new Bow();
            default: return new Gun(); // ค่าเริ่มต้น
        }
    }
};

#endif