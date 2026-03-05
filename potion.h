#ifndef POTION_H
#define POTION_H
#include <iostream>
using namespace std;

class Potion {
public:
    int atkBuffTurn  = 0;
    int critBuffTurn = 0;

    int invHP   = 0;
    int invAtk  = 0;
    int invCrit = 0;

    //potion HP
    void useHP(int& hp, int maxHp) {
        if (invHP <= 0) return;
        int heal = (rand() % 21) + 10;
        hp = (hp + heal > maxHp) ? maxHp : hp + heal;
        invHP--;
    }

    //potion Atk
    void useAtk() {
        if (invAtk <= 0) return;
        atkBuffTurn = 4;
        invAtk--;
    }

    // potion Crit
    void useCrit() {
        if (invCrit <= 0) return;
        critBuffTurn = 4;
        invCrit--;
    }

    void updateBuffs() {
        if (atkBuffTurn  > 0) atkBuffTurn--;
        if (critBuffTurn > 0) critBuffTurn--;
    }
};
#endif