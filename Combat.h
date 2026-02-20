#ifndef COMBAT_H
#define COMBAT_H

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Monster.h"
#include "Potion.h"

using namespace std;

struct Summary {
    int turns = 0, spent = 0, used = 0, totalDmg = 0;
};

class Combat {
public:
    static bool start(Player& p, Monster& m, Potion& pot, Summary& s, sf::RenderWindow& window) {
        bool isCrit;
        while (p.isAlive() && m.hp > 0 && window.isOpen()) {
            bool playerTurnActive = true; 
            s.turns++; 
            bool showMenu = true; 

            while (playerTurnActive && window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) window.close();
                }

                if (showMenu) {
                    cout << "\n===============================" << endl;
                    cout << "[ TURN " << s.turns << " ] " << m.name << " HP: " << m.hp << endl;
                    cout << "Player HP: " << p.hp << "/" << p.maxHp << " | Gold: " << p.wallet.balance << " G" << endl;
                    cout << "Inventory: HP[" << p.invHP << "] Atk[" << p.invAtk << "] Crit[" << p.invCrit << "]" << endl;
                    cout << "-------------------------------" << endl;
                    cout << ">> [C] Attack | [V] Use HP Pot" << endl;
                    cout << ">> SHOP: [1] Buy HP(25G) [2] Buy Atk(25G) [3] Buy Crit(25G) [4] Upgrade(50G)" << endl;
                    showMenu = false; 
                }

                // --- 1. โจมตี ---
            if(event.type == sf::Event::KeyPressed){
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                    int d = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                    m.hp -= d; s.totalDmg += d;
                    cout << ">> You dealt " << (isCrit ? "[CRITICAL!] " : "") << d << " dmg." << endl;
                    pot.updateBuffs(); 
                    playerTurnActive = false;
                    sf::sleep(sf::milliseconds(300)); 
                } 
                // --- 2. ใช้ยา HP ---
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
                    if (p.invHP > 0) {
                        pot.useHP(p.hp, p.maxHp); p.invHP--; s.used++;
                        cout << ">> Used HP Potion!" << endl;
                        playerTurnActive = false;
                    } else { cout << "!! No HP Potion !!" << endl; sf::sleep(sf::milliseconds(200)); showMenu = true; }
                    sf::sleep(sf::milliseconds(300));
                }
                // --- 3. ร้านค้า (กดแล้วไม่จบเทิร์น เพื่อให้ซื้อหลายอย่างได้) ---
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) { // ซื้อ HP
                    if (p.wallet.spendMoney(25)) { p.invHP++; s.spent += 25; cout << ">> Bought HP Pot!" << endl; showMenu = true; }
                    else { cout << "!! No Gold !!" << endl; showMenu = true; }
                    sf::sleep(sf::milliseconds(300));
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) { // ซื้อ Atk
                    if (p.wallet.spendMoney(25)) { p.invAtk++; s.spent += 25; cout << ">> Bought Atk Pot!" << endl; showMenu = true; }
                    else { cout << "!! No Gold !!" << endl; showMenu = true; }
                    sf::sleep(sf::milliseconds(300));
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) { // ซื้อ Crit
                    if (p.wallet.spendMoney(25)) { p.invCrit++; s.spent += 25; cout << ">> Bought Crit Pot!" << endl; showMenu = true; }
                    else { cout << "!! No Gold !!" << endl; showMenu = true; }
                    sf::sleep(sf::milliseconds(300));
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) { // อัปเกรด
                    if (p.wallet.spendMoney(50)) { p.weapon->upgrade(); s.spent += 50; cout << ">> Weapon Upgraded!" << endl; showMenu = true; }
                    else { cout << "!! No Gold !!" << endl; showMenu = true; }
                    sf::sleep(sf::milliseconds(300));
                }
            }

                window.clear();
                window.display();
            }
            if (m.hp <= 0) break;
            sf::sleep(sf::milliseconds(600));
            int md = (rand() % m.maxDmg) + 1; p.hp -= md;
            cout << ">> Monster counter-attacks for " << md << " dmg." << endl;
        }
        return p.isAlive();
    }
};
#endif