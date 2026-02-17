#ifndef BOW_H
#define BOW_H
#include "weapon.h"

class Bow : public Weapon {
public:
    Bow() {
        name = "Bow";
        baseBonusDmg = 5;
        bonusCrit = 20;
    }
};
#endif // BOW_H