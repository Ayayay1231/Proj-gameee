#ifndef BASEBALL_H
#define BASEBALL_H
#include "Weapon.h"
#include "Monster.h" // <--- ต้องมีบรรทัดนี้!

class BaseballBat : public Weapon {
public:
    BaseballBat() {
        name         = "Baseball Bat";
        baseBonusDmg = 5;
        bonusCrit    = 15;
    }

    void applyEffect(Monster& m) override {
        int chance = (level == 1) ? 20 : (level == 2) ? 35 : 50;
        if (rand() % 100 < chance) {
            m.stunTurn = (level == 3) ? 2 : 1; 
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