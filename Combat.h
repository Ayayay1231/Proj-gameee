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

        // ==========================================
        // 1. ดึงสกิลที่สวมใส่อยู่มาสร้างเป็นปุ่ม GUI
        // ==========================================
        Skill currentSkill = p.learnedSkills[p.equippedSkillIndex];
        string skillBtnText = "[ " + currentSkill.name + " (" + to_string(currentSkill.mpCost) + "MP) ]";

        sf::Text btnAttack("[ Attack ]", font, 24);         btnAttack.setPosition(70.f, 630.f);
        sf::Text btnSkill(skillBtnText, font, 24);          btnSkill.setPosition(210.f, 630.f); 
        sf::Text btnHeal("[ Heal ]", font, 24);             btnHeal.setPosition(480.f, 630.f);
        sf::Text btnBuy("[ Buy Pot ]", font, 24);           btnBuy.setPosition(610.f, 630.f);
        sf::Text btnTalk("[ Talk ]", font, 24);             btnTalk.setPosition(780.f, 630.f);
        
        // สร้างปุ่ม GUI สำหรับ State 4 (ตอนคุยกับมอนสเตอร์)
        sf::Text opt1("-> Apologize", font, 26);     opt1.setPosition(80.f, 630.f);
        sf::Text opt2("-> Intimidate", font, 26);    opt2.setPosition(300.f, 630.f);


        sf::Texture bgTexture;
        sf::Sprite bgSprite;
        bool hasBattleBg = false;
        if (bgTexture.loadFromFile("bgBattle.png")) { 
            bgSprite.setTexture(bgTexture);
            bgSprite.setScale(1280.f / bgTexture.getSize().x, 720.f / bgTexture.getSize().y);
            hasBattleBg = true;
        }

        s.turns = 1; 

        string fullText = "A wild " + m.name + " appeared!\n\n[ Click to continue ]";
        string currentText = "";
        int charIndex = 0;
        sf::Clock textClock;
        int textSpeed = 15; 

        while (p.isAlive() && window.isOpen()) {
            sf::Event event;
            
            // ดึงตำแหน่งเมาส์บนจอ
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                bool isAdvance = (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) ||
                                 (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);

                // ==========================================
                // 2. ดักจับการใช้เมาส์กดปุ่ม
                // ==========================================
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    
                    if (combatState == 1) {
                        bool actionTaken = false;
                        string nextLog = "";

                        // โจมตีธรรมดา
                        if (btnAttack.getGlobalBounds().contains(mousePosF)) {
                            int d = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                            m.hp -= d; s.totalDmg += d;
                            nextLog = "You dealt " + to_string(d) + " dmg " + (isCrit ? "[CRIT!]" : "") + "!\n\n";
                            actionTaken = true; pot.updateBuffs(); 
                        } 
                        // ==========================================
                        // ระบบใช้งานสกิลที่สวมใส่มา
                        // ==========================================
                        else if (btnSkill.getGlobalBounds().contains(mousePosF)) {
                            if (p.mp >= currentSkill.mpCost) { 
                                p.mp -= currentSkill.mpCost; 
                                
                                if (currentSkill.healAmt > 0) {
                                    p.hp += currentSkill.healAmt;
                                    if(p.hp > p.maxHp) p.hp = p.maxHp; 
                                    nextLog = "You used [" + currentSkill.name + "]! Healed " + to_string(currentSkill.healAmt) + " HP!\n\n";
                                } 
                                else {
                                    int baseDmg = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                                    int finalDmg = baseDmg * currentSkill.dmgMult; 
                                    
                                    m.hp -= finalDmg; 
                                    s.totalDmg += finalDmg;
                                    nextLog = "You used [" + currentSkill.name + "]! Dealt " + to_string(finalDmg) + " dmg " + (isCrit ? "[CRIT!]" : "") + "!\n\n";
                                }
                                actionTaken = true; pot.updateBuffs(); 
                            } else {
                                nextLog = "!! Not enough MP !!\n(Need " + to_string(currentSkill.mpCost) + " MP)\n\n"; 
                                actionTaken = true; 
                            }
                        }
                        // ==========================================
                        else if (btnHeal.getGlobalBounds().contains(mousePosF)) {
                             if (pot.invHP > 0) { // เปลี่ยนจาก p.invHP เป็น pot.invHP
                                 pot.useHP(p.hp, p.maxHp); 
                                    // ไม่ต้องพิมพ์ p.invHP--; แล้ว เพราะใน pot.useHP มันหักยาให้เรียบร้อยแล้ว
                                 s.used++;
                                  nextLog = "You used an HP Potion!\n"; actionTaken = true;
                            } else { 
                                 nextLog = "!! No HP Potion left !!\n\n"; actionTaken = true; 
                                 }
                        }

                        
                        else if (btnBuy.getGlobalBounds().contains(mousePosF)) {
                            if (p.wallet.spendMoney(25)) { 
                                p.invHP++; s.spent += 25; nextLog = "Bought HP Potion!\n\n"; 
                            } else { 
                                nextLog = "!! Not enough Gold !!\n\n"; 
                            }
                            actionTaken = true;
                        }
                        else if (btnTalk.getGlobalBounds().contains(mousePosF)) {
                            combatState = 4;
                            fullText = m.name + ": \"Grrrr... Why are you in my territory?!\""; 
                            currentText = ""; charIndex = 0; textClock.restart();
                            continue; 
                        }

                        if (actionTaken) {
                            combatState = 2;
                            fullText = nextLog + "[ Click to continue ]"; 
                            currentText = ""; charIndex = 0; textClock.restart();
                        }
                    }
                    else if (combatState == 4 && charIndex >= fullText.length()) { 
                        if (opt1.getGlobalBounds().contains(mousePosF)) {
                            int roll = rand() % 100;
                            if (roll > 40) {
                                fullText = m.name + ": \"Oh... alright then. Be careful next time!\"\n(The monster leaves peacefully)\n\n[ Click to end combat ]";
                                combatState = 5; 
                            } else {
                                fullText = m.name + ": \"Lies! I don't trust humans!\"\n\n[ Click to continue ]";
                                combatState = 6; 
                            }
                            currentText = ""; charIndex = 0; textClock.restart();
                        } 
                        else if (opt2.getGlobalBounds().contains(mousePosF)) {
                            fullText = m.name + ": \"YOU DARE THREATEN ME?! RAAWR!\"\n(Monster's attack increased!)\n\n[ Click to continue ]";
                            m.maxDmg += 5;
                            combatState = 6; 
                            currentText = ""; charIndex = 0; textClock.restart();
                        }
                    }
                }

                // ==========================================
                // 3. ระบบ Advance (คลิกเพื่อเล่นข้อความต่อ)
                // ==========================================
                if (isAdvance) {
                    if (combatState == 0) {
                        if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); } 
                        else { combatState = 1; }
                    }
                    else if (combatState == 2) {
                        if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); } 
                        else {
                            if (m.hp <= 0) return p.isAlive(); 
                            
                            combatState = 3; 
                            currentText = ""; charIndex = 0; textClock.restart();

                            if (m.isBoss && !m.ultiUsed && m.hp <= m.ultiThreshold) {
                                m.ultiUsed = true; 
                                p.hp -= m.ultiDmg;
                                fullText = "WARNING! " + m.name + " is enraged!\nIt uses [" + m.ultiName + "]! You took " + to_string(m.ultiDmg) + " dmg!\n\n[ Click to continue ]";
                            } 
                            else {
                                int md = (rand() % m.maxDmg) + 1; 
                                p.hp -= md;
                                fullText = m.name + " counter-attacks for " + to_string(md) + " dmg!\n\n[ Click to continue ]";
                            }
                        }
                    }
                    else if (combatState == 3) {
                        if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); } 
                        else {
                            if (p.hp <= 0) return p.isAlive(); 
                            s.turns++; combatState = 1; 
                        }
                    }
                    else if (combatState == 4) {
                            if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); }
                    }
                    else if (combatState == 5) {
                        if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); } 
                        else { m.hp = 0; return p.isAlive(); }
                    }
                    else if (combatState == 6) {
                        if (charIndex < fullText.length()) { currentText = fullText; charIndex = fullText.length(); } 
                        else {
                            int md = (rand() % m.maxDmg) + 1; p.hp -= md;
                            combatState = 3; 
                            fullText = m.name + " attacks you for " + to_string(md) + " dmg!\n\n[ Click to continue ]";
                            currentText = ""; charIndex = 0; textClock.restart();
                        }
                    }
                }

            } 

            if (combatState != 1 && combatState != 4) { 
                if (charIndex < fullText.length() && textClock.getElapsedTime().asMilliseconds() > textSpeed) {
                    currentText += fullText[charIndex]; charIndex++; textClock.restart(); 
                }
            }
            if (combatState == 4) {
                if (charIndex < fullText.length() && textClock.getElapsedTime().asMilliseconds() > textSpeed) {
                    currentText += fullText[charIndex]; charIndex++; textClock.restart(); 
                }
            }

            // ==========================================
            // 4. เอฟเฟกต์เมาส์ชี้ (Hover)
            // ==========================================
            btnAttack.setFillColor(btnAttack.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            btnSkill.setFillColor(btnSkill.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White); 
            btnHeal.setFillColor(btnHeal.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            btnBuy.setFillColor(btnBuy.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            btnTalk.setFillColor(btnTalk.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);

            opt1.setFillColor(opt1.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            opt2.setFillColor(opt2.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);

            // ==========================================
            // วาดภาพลงจอ
            // ==========================================
            window.clear(); 
            if (hasBattleBg) {
                window.draw(bgSprite);
            }
            window.draw(m.sprite);
            window.draw(menuboxes);
            
            if (combatState == 1) {
                string uistring = "[ Turn " + to_string(s.turns) + " ]\n";
                uistring += "Monster: " + m.name + " | HP: " + to_string(m.hp) + "/" + to_string(m.maxHp) + "\n\n";
                
                uistring += "Player HP: " + to_string(p.hp) + "/" + to_string(p.maxHp) + " | MP: " + to_string(p.mp) + "/" + to_string(p.maxMp) + " | Gold: " + to_string(p.wallet.balance) + " G\n";
                
                uistring += "Inventory -> HP Pots: " + to_string(p.invHP) + "\n";
                
                tui.setString(uistring);
                tui.setPosition(60.f, 470.f);
                window.draw(tui);

                window.draw(btnAttack);
                window.draw(btnSkill); 
                window.draw(btnHeal);
                window.draw(btnBuy);
                window.draw(btnTalk);
            
            } else if (combatState == 4) {
                tlog.setString(">> " + currentText);
                tlog.setPosition(60.f, 470.f); 
                window.draw(tlog);
                
                if (charIndex >= fullText.length()) {
                    window.draw(opt1);
                    window.draw(opt2);
                }
            } else {
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