#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "Weapon.h"
#include "Wallet.h"

using namespace std;

class Player {
public:
    // --- สเตตัสพื้นฐาน ---
    int hp;
    int maxHp;
    int baseMaxDmg    = 10;
    int baseCritChance = 5;

    // --- ระบบเลเวล ---
    int level = 1;
    int exp   = 0;

    // --- อุปกรณ์และการเงิน ---
    Weapon* weapon = nullptr;
    Wallet  wallet;

    // --- Inventory ---
    int invHP   = 0;
    int invAtk  = 0;
    int invCrit = 0;

    // ✅ ธนูไฟ
    bool hasFireArrow  = false;
    int  fireArrowAmmo = 0;

    bool isAlive() { return hp > 0; }

    int getNextLevelExp() { return level * 100; }

    int attack(bool& isCrit, int bonusDmg = 0, int bonusCrit = 0) {
        int totalMaxDmg    = baseMaxDmg + bonusDmg;
        int totalCritChance = baseCritChance + bonusCrit;

        if (weapon != nullptr)
            totalMaxDmg += weapon->getTotalBonusDmg();

        isCrit = (rand() % 100) < totalCritChance;
        int damage = (rand() % totalMaxDmg) + 1;
        if (isCrit) damage *= 2;

        return damage;
    }
};

#endif