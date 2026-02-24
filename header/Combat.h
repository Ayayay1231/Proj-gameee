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
        int combatState = 0;

        sf::Font font;
        if(!font.loadFromFile("Minecraft.ttf")){
            cout << "Cannot load font ";
        }

        sf::Text tui("", font, 24);
        tui.setFillColor(sf::Color::White);

        sf::Text tlog("", font, 24);
        tlog.setFillColor(sf::Color::Yellow);

        sf::RectangleShape menuboxes(sf::Vector2f(1200.f,250.f));
        menuboxes.setFillColor(sf::Color(20,20,20,200));
        menuboxes.setOutlineThickness(5.f);
        menuboxes.setOutlineColor(sf::Color::White);
        menuboxes.setPosition(40.f,450.f);

        
        
        s.turns = 1;

        // -----------------------------------------
        // 1. ตัวแปรสำหรับทำเอฟเฟกต์พิมพ์ดีด
        // -----------------------------------------
        string fullText = "A wild " + m.name + " appeared!\n\n[ Press SPACE to continue ]";
        string currentText = "";
        int charIndex = 0;
        sf::Clock textClock;
        int textSpeed = 15; // ความเร็วในการพิมพ์ (ค่ายิ่งน้อย ยิ่งพิมพ์เร็ว)

        while (p.isAlive() && window.isOpen()) {
            sf::Event event;
            
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed) {
                    
                    // State 0: เริ่มเกม
                    if (combatState == 0) {
                        if (event.key.code == sf::Keyboard::Space) {
                            // ถ้ายาวังพิมพ์ไม่เสร็จ กด Space เพื่อแสดงข้อความเต็มทีเดียว (Skip)
                            if (charIndex < fullText.length()) {
                                currentText = fullText;
                                charIndex = fullText.length();
                            } else {
                                // ถ้าพิมพ์เสร็จแล้ว กด Space เพื่อไปหน้าถัดไป
                                combatState = 1; 
                            }
                        }
                    }
                    // State 1: หน้าเมนูรอรับคำสั่ง (ไม่มีพิมพ์ดีด)
                    else if (combatState == 1) {
                        bool actionTaken = false;
                        string nextLog = "";

                        if (event.key.code == sf::Keyboard::C) {
                            int d = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                            m.hp -= d; 
                            s.totalDmg += d;
                            nextLog = "You dealt " + to_string(d) + " dmg " + (isCrit ? "[CRIT!]" : "") + "!\n\n";
                            pot.updateBuffs(); 
                            actionTaken = true;
                        } 
                        else if (event.key.code == sf::Keyboard::V) {
                            if (p.invHP > 0) {
                                pot.useHP(p.hp, p.maxHp); 
                                p.invHP--; 
                                s.used++;
                                nextLog = "You used an HP Potion!\n";
                                actionTaken = true;
                            } else { 
                                nextLog = "!! No HP Potion left !!\n\n"; 
                                actionTaken = true; 
                            }
                        }
                        else if (event.key.code == sf::Keyboard::Num1) {
                            if (p.wallet.spendMoney(25)) { 
                                p.invHP++; s.spent += 25; 
                                nextLog = "Bought HP Potion!\n\n"; 
                            } else { 
                                nextLog = "!! Not enough Gold !!\n\n"; 
                            }
                            actionTaken = true;
                        }

                        // ถ้ายืนยันคำสั่งแล้ว ให้เตรียมข้อความและตัวแปรพิมพ์ดีดใหม่
                        if (actionTaken) {
                            combatState = 2;
                            fullText = nextLog;
                            currentText = "";
                            charIndex = 0;
                            textClock.restart();
                        }
                    }
                    // State 2: ผลลัพธ์ผู้เล่น
                    else if (combatState == 2) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < fullText.length()) {
                                currentText = fullText;
                                charIndex = fullText.length();
                            } else {
                                if (m.hp <= 0) return p.isAlive(); 
                                
                                int md = (rand() % m.maxDmg) + 1; 
                                p.hp -= md;
                                
                                combatState = 3; 
                                fullText = m.name + " counter-attacks for " + to_string(md) + " dmg!\n\n";
                                currentText = "";
                                charIndex = 0;
                                textClock.restart();
                            }
                        }
                    }
                    // State 3: ผลลัพธ์มอนสเตอร์
                    else if (combatState == 3) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < fullText.length()) {
                                currentText = fullText;
                                charIndex = fullText.length();
                            } else {
                                if (p.hp <= 0) return p.isAlive(); 
                                s.turns++;
                                combatState = 1; 
                            }
                        }
                    }
                } // จบ if KeyPressed
            } // จบ while pollEvent

            // -----------------------------------------
            // 2. ลอจิกอัปเดตตัวอักษรพิมพ์ดีด (ทำงานตลอดเวลา ยกเว้นหน้าเมนู)
            // -----------------------------------------
            if (combatState != 1) { 
                if (charIndex < fullText.length() && textClock.getElapsedTime().asMilliseconds() > textSpeed) {
                    currentText += fullText[charIndex];
                    charIndex++;
                    textClock.restart(); // รีเซ็ตนาฬิกาเพื่อรอพิมพ์ตัวต่อไป
                }
            }

            // ==========================================
            // ส่วน Rendering
            // ==========================================
            window.clear(sf::Color(50, 50, 80));
            
            window.draw(m.sprite);
            window.draw(menuboxes);
            
            if (combatState == 1) {
                string uistring = "[ Turn " + to_string(s.turns) + " ]\n";
                uistring += "Monster: " + m.name + " | HP: " + to_string(m.hp) + "/" + to_string(m.maxHp) + "\n\n";
                uistring += "Player HP: " + to_string(p.hp) + "/" + to_string(p.maxHp) + " | Gold: " + to_string(p.wallet.balance) + " G\n";
                uistring += "Inventory -> HP Pots: " + to_string(p.invHP) + "\n\n";
                uistring += "ACTIONS: [C] Attack | [V] Heal | [1] Buy HP Pot (25G)";
                
                tui.setString(uistring);
                tui.setPosition(60.f, 470.f);
                window.draw(tui);
            } else {
                // 3. วาดข้อความ currentText ที่กำลังค่อยๆ โผล่
                tlog.setString(">> " + currentText);
                tlog.setPosition(60.f, 470.f); 
                window.draw(tlog);
            }
            
            window.display();
        }
        
        return p.isAlive();
    }
};
#endif
