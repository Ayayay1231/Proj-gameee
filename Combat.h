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
    int turns = 0, used = 0, totalDmg = 0;
};

class Combat {
public:
    static bool start(Player& p, Monster& m, Potion& pot, Summary& s, sf::RenderWindow& window) {
        bool isCrit;
        int combatState = 0;

        sf::Font font;
        if (!font.loadFromFile("Minecraft.ttf"))
            cout << "Cannot load font\n";

        sf::Text tui("", font, 24);
        tui.setFillColor(sf::Color::White);

        sf::Text tlog("", font, 24);
        tlog.setFillColor(sf::Color::Yellow);

        sf::RectangleShape menuboxes(sf::Vector2f(1200.f, 250.f));
        menuboxes.setFillColor(sf::Color(20, 20, 20, 200));
        menuboxes.setOutlineThickness(5.f);
        menuboxes.setOutlineColor(sf::Color::White);
        menuboxes.setPosition(40.f, 450.f);

        s.turns = 1;
        string fullText    = "A wild " + m.name + " appeared!\n\n[ Press SPACE to continue ]";
        string currentText = "";
        int    charIndex   = 0;
        sf::Clock textClock;
        int textSpeed = 15;

        while (p.isAlive() && window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {

                    // ─── State 0: Intro ───────────────────────────────────
                    if (combatState == 0) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < (int)fullText.length()) {
                                currentText = fullText;
                                charIndex   = fullText.length();
                            } else {
                                combatState = 1;
                            }
                        }
                    }

                    // ─── State 1: เมนูรับคำสั่ง ──────────────────────────
                    else if (combatState == 1) {
                        bool   actionTaken = false;
                        string nextLog     = "";

                        // [C] โจมตีธรรมดา
                        if (event.key.code == sf::Keyboard::C) {
                            int d       = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                            int actualD = (int)(d * m.getDefMultiplier());
                            m.hp -= actualD;
                            s.totalDmg += actualD;
                            nextLog = "You dealt " + to_string(actualD) + " dmg " + (isCrit ? "[CRIT!]" : "") + "!\n";

                            // สุ่มติด infected 30%
                            bool wasInfected = (m.infectedTurn > 0);
                            m.tryInflictInfected();
                            if (!wasInfected && m.infectedTurn > 0)
                                nextLog += ">> " + m.name + " is now INFECTED!\n";

                            nextLog += "\n";
                            pot.updateBuffs();
                            actionTaken = true;
                        }

                        // [F] ยิงธนูไฟ
                        else if (event.key.code == sf::Keyboard::F) {
                            if (!p.hasFireArrow) {
                                nextLog = "!! You don't have Fire Arrows !!\n\n";
                            } else if (p.fireArrowAmmo <= 0) {
                                nextLog = "!! No Fire Arrows left !!\n\n";
                            } else {
                                int d       = p.attack(isCrit, (pot.atkBuffTurn > 0 ? 10 : 0), (pot.critBuffTurn > 0 ? 10 : 0));
                                int actualD = (int)(d * m.getDefMultiplier());
                                m.hp -= actualD;
                                s.totalDmg += actualD;
                                p.fireArrowAmmo--;
                                pot.updateBuffs();

                                nextLog = "Fire Arrow! You dealt " + to_string(actualD) + " dmg "
                                        + (isCrit ? "[CRIT!]" : "") + "!\n";

                                bool wasBurning = (m.burnTurn > 0);
                                m.tryInflictBurn();
                                if (!wasBurning && m.burnTurn > 0)
                                    nextLog += ">> " + m.name + " is now BURNING!\n";
                                else if (wasBurning)
                                    nextLog += ">> " + m.name + " is already burning!\n";
                                else
                                    nextLog += ">> The fire missed... (no burn)\n";

                                nextLog += "Ammo left: " + to_string(p.fireArrowAmmo) + "\n\n";
                            }
                            actionTaken = true;
                        }

                        // [V] ใช้ HP Potion
                        else if (event.key.code == sf::Keyboard::V) {
                            if (pot.invHP > 0) {
                                pot.useHP(p.hp, p.maxHp);
                                s.used++;
                                nextLog = "Used HP Potion!\n\n";
                            } else {
                                nextLog = "!! No HP Potion left !!\n\n";
                            }
                            actionTaken = true;
                        }

                        // [Q] ใช้ ATK Potion
                        else if (event.key.code == sf::Keyboard::Q) {
                            if (pot.atkBuffTurn > 0) {
                                nextLog = "!! ATK Buff already active! (" + to_string(pot.atkBuffTurn) + " turns left)\n\n";
                            } else if (pot.invAtk > 0) {
                                pot.useAtk();
                                s.used++;
                                nextLog = "Used ATK Potion! DMG +10 for 4 turns!\n\n";
                            } else {
                                nextLog = "!! No ATK Potion left !!\n\n";
                            }
                            actionTaken = true;
                        }

                        // [E] ใช้ CRIT Potion
                        else if (event.key.code == sf::Keyboard::E) {
                            if (pot.critBuffTurn > 0) {
                                nextLog = "!! CRIT Buff already active! (" + to_string(pot.critBuffTurn) + " turns left)\n\n";
                            } else if (pot.invCrit > 0) {
                                pot.useCrit();
                                s.used++;
                                nextLog = "Used CRIT Potion! CRIT +10% for 4 turns!\n\n";
                            } else {
                                nextLog = "!! No CRIT Potion left !!\n\n";
                            }
                            actionTaken = true;
                        }

                        if (actionTaken) {
                            combatState = 2;
                            fullText    = nextLog;
                            currentText = "";
                            charIndex   = 0;
                            textClock.restart();
                        }
                    }

                    // ─── State 2: ผลลัพธ์ผู้เล่น ─────────────────────────
                    else if (combatState == 2) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < (int)fullText.length()) {
                                currentText = fullText;
                                charIndex   = fullText.length();
                            } else {
                                if (m.hp <= 0) {
                                    if (m.shouldSpawnOnDeath()) {
                                        fullText    = "The rotting corpse spawns a Mini Zombie!\n\n[ SPACE to continue ]";
                                        currentText = "";
                                        charIndex   = 0;
                                        textClock.restart();
                                        combatState = 4;
                                        break;
                                    }
                                    return p.isAlive();
                                }

                                string statusLog = m.applyStatusEffects();
                                if (m.hp <= 0) return p.isAlive();

                                int totalMd = 0;
                                int hits    = m.getAttackCount();
                                for (int i = 0; i < hits; i++) {
                                    if (m.stunTurn > 0) {
                                        m.stunTurn--;
                                        statusLog += m.name + " is STUNNED and can't attack!\n";
                                        break;
                                    }
                                    totalMd += (rand() % m.getActualDmg()) + 1;
                                }
                                p.hp -= totalMd;

                                string hitLabel = (hits > 1) ? " (x" + to_string(hits) + " FRENZY!)" : "";
                                combatState = 3;
                                fullText    = statusLog + m.name + " counter-attacks for "
                                            + to_string(totalMd) + " dmg!" + hitLabel + "\n\n";
                                currentText = "";
                                charIndex   = 0;
                                textClock.restart();
                            }
                        }
                    }

                    // ─── State 3: ผลลัพธ์มอนสเตอร์ ──────────────────────
                    else if (combatState == 3) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < (int)fullText.length()) {
                                currentText = fullText;
                                charIndex   = fullText.length();
                            } else {
                                if (p.hp <= 0) return p.isAlive();
                                s.turns++;
                                combatState = 1;
                            }
                        }
                    }

                    // ─── State 4: spawn mini zombie ───────────────────────
                    else if (combatState == 4) {
                        if (event.key.code == sf::Keyboard::Space) {
                            if (charIndex < (int)fullText.length()) {
                                currentText = fullText;
                                charIndex   = fullText.length();
                            } else {
                                return p.isAlive();
                            }
                        }
                    }

                } // จบ KeyPressed
            } // จบ pollEvent

            // ─── Typewriter ───────────────────────────────────────────────
            if (combatState != 1) {
                if (charIndex < (int)fullText.length() &&
                    textClock.getElapsedTime().asMilliseconds() > textSpeed) {
                    currentText += fullText[charIndex];
                    charIndex++;
                    textClock.restart();
                }
            }

            // ─── Rendering ────────────────────────────────────────────────
            window.clear(sf::Color(50, 50, 80));
            m.draw(window);
            window.draw(menuboxes);

            if (combatState == 1) {
                string statusIcons = "";
                if (m.infectedTurn > 0) statusIcons += " [INF x" + to_string(m.infectedTurn) + "]";
                if (m.burnTurn     > 0) statusIcons += " [BURN x" + to_string(m.burnTurn)     + "]";
                if (m.rotten)           statusIcons += " [ROT]";
                if (m.frenzy)           statusIcons += " [FRNZ]";

                // buff icons
                string buffIcons = "";
                if (pot.atkBuffTurn  > 0) buffIcons += " [ATK+" + to_string(pot.atkBuffTurn)  + "]";
                if (pot.critBuffTurn > 0) buffIcons += " [CRIT+" + to_string(pot.critBuffTurn) + "]";

                string uistring = "[ Turn " + to_string(s.turns) + " ]\n";
                uistring += "Monster: " + m.name + " | HP: " + to_string(m.hp) + "/" + to_string(m.maxHp);
                uistring += statusIcons + "\n\n";
                uistring += "Player HP: " + to_string(p.hp) + "/" + to_string(p.maxHp)
                          + " | Gold: " + to_string(p.wallet.balance) + " G" + buffIcons + "\n";
                uistring += "Bag: HP Pot x" + to_string(pot.invHP)
                          + " | ATK Pot x" + to_string(pot.invAtk)
                          + " | CRIT Pot x" + to_string(pot.invCrit);
                if (p.hasFireArrow)
                    uistring += " | Fire Arrows x" + to_string(p.fireArrowAmmo);
                uistring += "\n\n";
                uistring += "ACTIONS: [C] Attack | [V] HP Pot | [Q] ATK Pot | [E] CRIT Pot";
                if (p.hasFireArrow)
                    uistring += " | [F] Fire Arrow";

                tui.setString(uistring);
                tui.setPosition(60.f, 470.f);
                window.draw(tui);
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