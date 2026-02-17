#ifndef BASEBALL_H
#define BASEBALL_H
#include "weapon.h"

class BaseballBat : public Weapon {
public:
    BaseballBat() {
        name = "Baseball Bat";
        baseBonusDmg = 5;
        bonusCrit = 15;
    }
};
#endif
