#ifndef SWORD_H
#define SWORD_H
#include "weapon.h"

class Sword : public Weapon {
public:
    Sword() {
        name         = "Sword";
        baseBonusDmg = 15;
        bonusCrit    = 5;
    }

    void applyEffect(Monster& m) override {
        // Lv.1=20%  Lv.2=35%  Lv.3=50%
        int chance = (level == 1) ? 20 : (level == 2) ? 35 : 50;
        if (rand() % 100 < chance) {
            m.bleedTurn = 3;
            cout << ">> [EFFECT] The monster is BLEEDING!" << endl;
        }
    }

    int getTotalBonusDmg(Monster& m, bool& isCrit) override {
        int dmg = baseBonusDmg + (level * 5);
        if (m.bleedTurn > 0)
            dmg += (level == 3) ? 10 : 5; // Lv.3 bonus เพิ่ม
        return dmg;
    }
};
#endif