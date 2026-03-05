#ifndef BOW_H
#define BOW_H
#include "Weapon.h"
#include "Monster.h" // <--- ต้องมีบรรทัดนี้!

class Bow : public Weapon {
public:
    Bow() {
        name         = "Bow";
        baseBonusDmg = 5;
        bonusCrit    = 20;
    }

    void applyEffect(Monster& m) override {
        if (level < 3) {
            int chance = (level == 1) ? 20 : 35;
            if (rand() % 100 < chance) {
                m.vulnTurn = 3;
                cout << ">> [EFFECT] The monster is VULNERABLE!" << endl;
            }
        } else {
            m.burnTurn = 3;
            cout << ">> [EFFECT] The monster is BURNING!" << endl;
        }
    }

    int getTotalBonusDmg(Monster& m, bool& isCrit) override {
        int dmg = baseBonusDmg + (level * 5);
        if (level < 3) {
            if (m.vulnTurn > 0) dmg += 15;
        } else {
            if (m.burnTurn > 0) dmg += 20;
        }
        return dmg;
    }

    bool isFireArrowUnlocked() const {
        return level >= 3;
    }
};
#endif