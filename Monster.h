#ifndef MONSTER_H
#define MONSTER_H
#include <string>
using namespace std;

class Monster {
public:
    string name;
    int hp, maxHp, maxDmg; // เพิ่ม maxHp เข้าไปตรงนี้

    Monster(int rank) {
        name = "Monster #" + to_string(rank);
        maxHp = 50 + (rank * 10);
        hp = maxHp;
        maxDmg = 5 + (rank * 2);
    }
};
#endif