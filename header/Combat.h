#ifndef COMBAT_H
#define COMBAT_H

#include <iostream>
#include <string>
#include <SFML/System.hpp>
#include "Player.h"
#include "Monster.h"
#include "Potion.h"

using namespace std;

// Struct สำหรับเก็บสถิติหลังจบการต่อสู้
struct Summary {
    int turns = 0, spent = 0, used = 0, totalDmg = 0;
};

class Combat {
public:
    static bool start(Player& p, Monster& m, Potion& pot, Summary& s) {
        char act; bool isCrit;
        
        while (p.isAlive() && m.hp > 0) {
            bool playerTurnActive = true; 
            
            // 1. นับเทิร์นเมื่อเริ่มรอบใหม่
            s.turns++; 
            
            while (playerTurnActive) {
                cout << "\n===============================" << endl;
                cout << "[ TURN " << s.turns << " ] " << m.name << " HP: " << m.hp << endl;
                cout << "Player LV: " << p.level << " (EXP: " << p.exp << "/" << p.getNextLevelExp() << ")" << endl;
                cout << "Player HP: " << p.hp << "/" << p.maxHp << endl;
                cout << "Gold: " << p.wallet.balance << " G" << endl; // แสดงเงิน
                
                cout << "--- ACTIVE BUFFS ---" << endl;
                cout << ">> [ATK UP]  Remaining: " << (pot.atkBuffTurn > 0 ? to_string(pot.atkBuffTurn) : "None") << endl;
                cout << ">> [CRIT UP] Remaining: " << (pot.critBuffTurn > 0 ? to_string(pot.critBuffTurn) : "None") << endl;
                cout << "-------------------------------" << endl;
                
                cout << "Inventory: HP[" << p.invHP << "] Atk[" << p.invAtk << "] Crit[" << p.invCrit << "]" << endl;
                cout << "Actions: [c] Attack | [v] Use | [b] Shop/Upgrade: "; cin >> act;

                if (act == 'b') {
                    cout << "\n--- SHOP (Your Gold: " << p.wallet.balance << " G) ---" << endl;
                    cout << "1.HP(25G) 2.Atk(25G) 3.Crit(25G) 4.Upgrade(50G) 0.Back: "; 
                    int i; cin >> i;
                    
                    if (i == 4) {
                        // เช็คเงินก่อนอัปเกรด
                        if (p.wallet.spendMoney(50)) { 
                            p.weapon->upgrade(); 
                            s.spent += 50; 
                            cout << ">> Weapon Upgraded!" << endl; 
                        } else {
                            cout << "!! Not enough gold (Need 50G) !!" << endl;
                        }
                    } else if (i >= 1 && i <= 3) {
                        // เช็คเงินก่อนซื้อยา
                        if (p.wallet.spendMoney(25)) {
                            s.spent += 25; 
                            if (i == 1) p.invHP++; else if (i == 2) p.invAtk++; else p.invCrit++;
                            cout << ">> Item Purchased!" << endl;
                        } else {
                            cout << "!! Not enough gold (Need 25G) !!" << endl;
                        }
                    }
                } else if (act == 'v') {
                    cout << "Use: 1.HP 2.Atk 3.Crit: "; int u; cin >> u;
                    if (u == 1 && p.invHP > 0) { pot.useHP(p.hp, p.maxHp); p.invHP--; playerTurnActive = false; s.used++; }
                    else if (u == 2 && p.invAtk > 0) { pot.atkBuffTurn = 3; p.invAtk--; playerTurnActive = false; s.used++; }
                    else if (u == 3 && p.invCrit > 0) { pot.critBuffTurn = 3; p.invCrit--; playerTurnActive = false; s.used++; }
                    else { cout << "!! No item or invalid choice !!" << endl; }
                } else if (act == 'c') {
                    // คำนวณดาเมจ
                    int d = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                    m.hp -= d; 
                    
                    // 2. เก็บสถิติดาเมจรวม
                    s.totalDmg += d; 
                    
                    cout << ">> You dealt " << (isCrit ? "[CRITICAL!] " : "") << d << " dmg." << endl;
                    pot.updateBuffs(); 
                    playerTurnActive = false;
                }
            }
            
            if (m.hp <= 0) break;
            
            // Monster Turn
            sf::sleep(sf::milliseconds(600));
            int md = (rand() % m.maxDmg) + 1; 
            p.hp -= md;
            cout << ">> Monster counter-attacks for " << md << " dmg." << endl;
        }
        return p.isAlive();
    }
};
#endif
