#ifndef PLAYERCOMBAT_H
#define PLAYERCOMBAT_H

#include"unit.h"

class PlayerC :
public Unit{
    public :
    int level , exp;

    PlayerC(std::string n, int h, int m, int a, int i,std::string image);

    void usePotion();

    void gainExp(int a);
};





#endif