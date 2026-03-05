#ifndef PLAYER_H
#define PLAYER_H

#include "Weapon.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Skill { string name; string desc; int mpCost; float dmgMult; int healAmt; };
struct Item { string name; string desc; int amount; int healHp; };
struct Quest { string name; string desc; bool isCompleted; int progress = 0; int maxProgress = 0; };

class Wallet {
public:
    int balance;
    Wallet() : balance(0) {}
    void addMoney(int m) { balance += m; }
    bool spendMoney(int m) { 
        if (balance >= m) { balance -= m; return true; }
        return false;
    }
};

class Player {
public:
    int hp, maxHp;
    int mp, maxMp; 
    int invHP = 0; 
    
    vector<Skill> learnedSkills; 
    int equippedSkillIndex = 0;  

    vector<Item> inventory; 
    vector<Quest> questLog; 

    int slimesKilled = 0; 

    // --- เพิ่มตัวแปรสำหรับร้านค้า (ธนูไฟ) ---
    bool hasFireArrow = false;
    int fireArrowAmmo = 0;

    int level, exp;
    int baseMaxDmg = 15; 
    Weapon* weapon = nullptr; 
    Wallet wallet;

    Player() {
        level = 1; exp = 0; maxHp = 100; hp = maxHp;
        maxMp = 50; mp = maxMp;
    }

    int getNextLevelExp() { return level * 50; }
    bool isAlive() { return hp > 0; }

    int attack(bool& isCrit, int atkBuff, int critBuff) {
        int wDmg = (weapon != nullptr) ? weapon->getDamage() : 0;
        int d = (rand() % (baseMaxDmg + wDmg + atkBuff)) + 5;
        
        int critChance = (weapon != nullptr) ? weapon->getCritChance() : 5;
        critChance += critBuff;
        
        isCrit = (rand() % 100 < critChance);
        if (isCrit) d *= 2; 
        return d;
    }
};
#endif