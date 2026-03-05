#ifndef BOW_H
#define BOW_H
#include "weapon.h"
#include "Monster.h"

class Bow : public Weapon {
public:
    Bow() {
        name         = "Bow";
        baseBonusDmg = 5;
        bonusCrit    = 20;
    }

    void applyEffect(Monster& m) override {
        // Lv.1=20%  Lv.2=35%
        // Lv.3 = Fire Arrow ทุกลูก ติด burn แทน vuln
        if (level < 3) {
            int chance = (level == 1) ? 20 : 35;
            if (rand() % 100 < chance) {
                m.vulnTurn = 3;
                cout << ">> [EFFECT] The monster is VULNERABLE!" << endl;
            }
        } else {
            // ✅ Lv.3: ทุกลูกติด burn 100% (Fire Arrow)
            m.burnTurn = 3;
            cout << ">> [EFFECT] The monster is BURNING!" << endl;
        }
    }

    int getTotalBonusDmg(Monster& m, bool& isCrit) override {
        int dmg = baseBonusDmg + (level * 5);
        if (level < 3) {
            if (m.vulnTurn > 0) dmg += 15;
        } else {
            // ✅ Lv.3: bonus dmg กับเป้าที่ burn อยู่
            if (m.burnTurn > 0) dmg += 20;
        }
        return dmg;
    }

    // ✅ เรียกจาก Game เพื่อเช็คว่าควร unlock Fire Arrow ไหม
    bool isFireArrowUnlocked() const {
        return level >= 3;
    }
};
#endif