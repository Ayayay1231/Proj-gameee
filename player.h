#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "Weapon.h"
#include "Wallet.h"

using namespace std;

class Player {
public:
    // --- สเตตัสพื้นฐาน ---
    int hp;
    int maxHp;
    int baseMaxDmg = 10;
    int baseCritChance = 5;

    // --- ระบบเลเวล (ประกาศครั้งเดียว) ---
    int level = 1;
    int exp = 0;

    // --- อุปกรณ์และการเงิน ---
    Weapon* weapon = nullptr;
    Wallet wallet;

    // --- ช่องเก็บไอเทม (Inventory) ---
    int invHP = 0;
    int invAtk = 0;
    int invCrit = 0;

    // --- ฟังก์ชันช่วยเหลือ ---
    
    // ตรวจสอบว่ายังมีชีวิตอยู่ไหม
    bool isAlive() {
        return hp > 0;
    }

    // คำนวณ EXP ที่ต้องการสำหรับเลเวลถัดไป (Level * 100)
    int getNextLevelExp() {
        return level * 100;
    }

    // ฟังก์ชันโจมตี (คำนวณร่วมกับอาวุธและบัฟ)
    int attack(bool& isCrit, int bonusDmg = 0, int bonusCrit = 0) {
        int totalMaxDmg = baseMaxDmg + bonusDmg;
        int totalCritChance = baseCritChance + bonusCrit;

        // ถ้ามีอาวุธ ให้บวกพลังโจมตีจากอาวุธเข้าไปด้วย
        if (weapon != nullptr) {
            totalMaxDmg += weapon->getTotalBonusDmg();
        }

        // สุ่มคำนวณคริติคอล
        isCrit = (rand() % 100) < totalCritChance;
        int damage = (rand() % totalMaxDmg) + 1;

        if (isCrit) {
            damage *= 2; // Damage 2 เท่าถ้าติดคริติคอล
        }

        return damage;
    }
};

#endif