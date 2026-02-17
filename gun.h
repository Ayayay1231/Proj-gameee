#ifndef GUN_H
#define GUN_H
#include "weapon.h"

class Gun : public Weapon {
public:
    Gun() {
        name = "Gun";
        baseBonusDmg = 15;
        bonusCrit = 5;
    }
};
#endif