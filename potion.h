#ifndef POTION_H
#define POTION_H
#include <iostream>
using namespace std;

class Potion {
public:
    int atkBuffTurn = 0;
    int critBuffTurn = 0;

    void useHP(int& hp, int maxHp) {
        int heal = (rand() % 21) + 10;
        hp = (hp + heal > maxHp) ? maxHp : hp + heal;
        cout << ">> Result: Healed " << heal << " HP!" << endl;
    }

    void useAtk() { atkBuffTurn = 4; cout << ">> Result: ATK Buff activated!" << endl; }
    void useCrit() { critBuffTurn = 4; cout << ">> Result: CRIT Buff activated!" << endl; }

    void updateBuffs() {
        if (atkBuffTurn > 0) atkBuffTurn--;
        if (critBuffTurn > 0) critBuffTurn--;
    }
};
#endif