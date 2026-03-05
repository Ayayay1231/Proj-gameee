#ifndef SHOP_H
#define SHOP_H
#include <string>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Potion.h"
#include "Weapon.h"

using namespace std;

class Shop {
public:
    static void open(Player& p, Potion& pot, sf::RenderWindow& window, sf::Font& font) {

        sf::RectangleShape box(sf::Vector2f(1000.f, 340.f));
        box.setFillColor(sf::Color(20, 20, 20, 220));
        box.setOutlineThickness(4.f);
        box.setOutlineColor(sf::Color::Yellow);
        box.setPosition(140.f, 350.f);

        sf::Text titleText("", font, 28);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setPosition(160.f, 360.f);
        titleText.setString("=== WANDERING MERCHANT ===");

        sf::Text menuText("", font, 22);
        menuText.setFillColor(sf::Color::White);
        menuText.setPosition(160.f, 400.f);

        sf::Text msgText("", font, 22);
        msgText.setFillColor(sf::Color::Green);
        msgText.setPosition(160.f, 660.f);

        string currentMsg = "Welcome! What will you buy?";

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {

                    if (event.key.code == sf::Keyboard::Num1) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invHP++;
                            currentMsg = "Bought HP Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    else if (event.key.code == sf::Keyboard::Num2) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invAtk++;
                            currentMsg = "Bought ATK Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    else if (event.key.code == sf::Keyboard::Num3) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invCrit++;
                            currentMsg = "Bought CRIT Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    else if (event.key.code == sf::Keyboard::Num4) {
                        if (!p.hasFireArrow) {
                            currentMsg = "!! Unlock Fire Arrows first (Bow Lv.3)";
                        } else if (p.wallet.spendMoney(20)) {
                            p.fireArrowAmmo += 3;
                            currentMsg = "Bought 3 Fire Arrows! (-20G)";
                        } else { currentMsg = "!! Not enough Gold (need 20G)"; }
                    }
                    else if (event.key.code == sf::Keyboard::Num5) {
                        if (!p.weapon) {
                            currentMsg = "!! No weapon equipped";
                        } else if (p.weapon->isMaxLevel()) {
                            currentMsg = "!! " + p.weapon->name + " is already MAX level!";
                        } else {
                            int cost = p.weapon->upgradeCost();
                            if (p.wallet.spendMoney(cost)) {
                                p.weapon->upgrade();
                                currentMsg = "Upgraded " + p.weapon->name
                                           + " to Lv." + to_string(p.weapon->level) + "! (-"
                                           + to_string(cost) + "G)";
                                Bow* bow = dynamic_cast<Bow*>(p.weapon);
                                if (bow && bow->isFireArrowUnlocked() && !p.hasFireArrow) {
                                    p.hasFireArrow  = true;
                                    p.fireArrowAmmo = 5;
                                    currentMsg += "\n>> Fire Arrows UNLOCKED! (+5 ammo)";
                                }
                            } else {
                                currentMsg = "!! Not enough Gold (need "
                                           + to_string(p.weapon->upgradeCost()) + "G)";
                            }
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        return;
                    }
                }
            }

            string weaponInfo = p.weapon
                ? (p.weapon->name + " Lv." + to_string(p.weapon->level) + "/3"
                   + (p.weapon->isMaxLevel() ? " [MAX]"
                      : "  ->Upgrade: " + to_string(p.weapon->upgradeCost()) + "G"))
                : "No weapon";

            string fireInfo = p.hasFireArrow
                ? ("x" + to_string(p.fireArrowAmmo) + " ammo  ->Refill x3: 20G")
                : "Locked (need Bow Lv.3)";

            string menu = "";
            menu += "[1] HP Potion     25G   (have: " + to_string(pot.invHP)   + ")\n";
            menu += "[2] ATK Potion    25G   (have: " + to_string(pot.invAtk)  + ")\n";
            menu += "[3] CRIT Potion   25G   (have: " + to_string(pot.invCrit) + ")\n";
            menu += "[4] Fire Arrow    20G   " + fireInfo + "\n";
            menu += "[5] Upgrade Weapon      " + weaponInfo + "\n\n";
            menu += "Gold: " + to_string(p.wallet.balance) + "G";
            if (p.hasFireArrow)
                menu += "  |  Fire Arrows: " + to_string(p.fireArrowAmmo);
            menu += "\n\n[ESC] Leave Shop";

            menuText.setString(menu);
            msgText.setString(currentMsg);

            window.clear(sf::Color(30, 30, 60));
            window.draw(box);
            window.draw(titleText);
            window.draw(menuText);
            window.draw(msgText);
            window.display();
        }
    }
};
#endif