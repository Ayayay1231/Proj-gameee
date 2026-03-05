#ifndef BASEBALL_H
#define BASEBALL_H
#include "weapon.h"
#include "Monster.h"

class BaseballBat : public Weapon {
public:
    BaseballBat() {
        name         = "Baseball Bat";
        baseBonusDmg = 5;
        bonusCrit    = 15;
    }

    void applyEffect(Monster& m) override {
        // Lv.1=20%  Lv.2=35%  Lv.3=50%
        int chance = (level == 1) ? 20 : (level == 2) ? 35 : 50;
        if (rand() % 100 < chance) {
            m.stunTurn = (level == 3) ? 2 : 1; // Lv.3 stun 2 เทิร์น
            cout << ">> [EFFECT] The monster is STUNNED!" << endl;
        }
    }

    int getTotalBonusDmg(Monster& m, bool& isCrit) override {
        int dmg = baseBonusDmg + (level * 5);
        if (m.stunTurn > 0)
            dmg += (level == 3) ? 15 : 10;
        return dmg;
    }
};
#endif