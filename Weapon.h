#ifndef WEAPON_H
#define WEAPON_H
#include <string>
#include <iostream>

using namespace std;

class Weapon {
public:
    string name;
    int level;
    int baseBonusDmg;
    int bonusCrit;
    int maxLevel = 3; 

    Weapon() : level(1), baseBonusDmg(0), bonusCrit(0), name("Unknown") {}
    virtual ~Weapon() {}

    virtual int getTotalBonusDmg() {
        return baseBonusDmg + (level * 5);
    }

    virtual int getTotalBonusDmg(class Monster& m, bool& isCrit) {
        return baseBonusDmg + (level * 5); 
    }

    // --- 2 ตัวนี้คือตัวที่ Player.h โวยวายว่าหาไม่เจอ! ---
    int getDamage() { return getTotalBonusDmg(); }
    int getCritChance() { return bonusCrit; }

    bool upgrade() {
        if (isMaxLevel()) return false;
        level++;
        return true;
    }

    bool isMaxLevel() { return level >= maxLevel; }

    int upgradeCost() {
        if (level == 1) return 50;
        if (level == 2) return 100;
        return 0;
    }

    virtual void applyEffect(class Monster& m) {}
};

#include "sword.h"
#include "baseball.h"
#include "bow.h"

class WeaponFactory {
public:
    static Weapon* selectWeapon(int choice) {
        switch (choice) {
            case 1:  return new Sword();
            case 2:  return new BaseballBat();
            case 3:  return new Bow();
            default: return new Sword();
        }
    }
};
#endif