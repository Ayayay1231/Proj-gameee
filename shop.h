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

        // ปรับขนาดกล่องและพิกัดให้อ่านง่ายขึ้น
        sf::RectangleShape box(sf::Vector2f(1000.f, 480.f));
        box.setFillColor(sf::Color(20, 20, 20, 230));
        box.setOutlineThickness(4.f);
        box.setOutlineColor(sf::Color::Yellow);
        box.setPosition(140.f, 120.f);

        sf::Text titleText("=== WANDERING MERCHANT ===", font, 28);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setPosition(160.f, 140.f);

        sf::Text goldText("", font, 24);
        goldText.setFillColor(sf::Color::Yellow);
        goldText.setPosition(750.f, 140.f);

        sf::Text msgText("Welcome! Click on an item to buy.", font, 24);
        msgText.setFillColor(sf::Color::Green);
        msgText.setPosition(160.f, 480.f);

        sf::Text escText("[ESC] Leave Shop", font, 20);
        escText.setFillColor(sf::Color(150, 150, 150));
        escText.setPosition(160.f, 540.f);

        // สร้าง Array ของข้อความเพื่อทำปุ่มให้คลิกได้ 5 ปุ่ม
        sf::Text opt[5];
        for(int i=0; i<5; i++){
            opt[i].setFont(font);
            opt[i].setCharacterSize(24);
            opt[i].setPosition(180.f, 210.f + (i * 50.f));
        }

        string currentMsg = "Welcome! Click on an item to buy.";

        while (window.isOpen()) {
            sf::Event event;
            
            // ดึงตำแหน่งเมาส์ในปัจจุบัน
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // กดปุ่ม ESC เพื่อออกยังอยู่เหมือนเดิม
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        return; 
                    }
                }
                
                // ดักจับการคลิกเมาส์ซ้าย
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    
                    // คลิกปุ่ม 1: ยา HP
                    if (opt[0].getGlobalBounds().contains(mousePosF)) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invHP++; currentMsg = "Bought HP Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    // คลิกปุ่ม 2: ยา ATK
                    else if (opt[1].getGlobalBounds().contains(mousePosF)) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invAtk++; currentMsg = "Bought ATK Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    // คลิกปุ่ม 3: ยา CRIT
                    else if (opt[2].getGlobalBounds().contains(mousePosF)) {
                        if (p.wallet.spendMoney(25)) {
                            pot.invCrit++; currentMsg = "Bought CRIT Potion! (-25G)";
                        } else { currentMsg = "!! Not enough Gold (need 25G)"; }
                    }
                    // คลิกปุ่ม 4: ธนูไฟ
                    else if (opt[3].getGlobalBounds().contains(mousePosF)) {
                        if (!p.hasFireArrow) {
                            currentMsg = "!! Unlock Fire Arrows first (Bow Lv.3)";
                        } else if (p.wallet.spendMoney(20)) {
                            p.fireArrowAmmo += 3; currentMsg = "Bought 3 Fire Arrows! (-20G)";
                        } else { currentMsg = "!! Not enough Gold (need 20G)"; }
                    }
                    // คลิกปุ่ม 5: อัปเกรดอาวุธ
                    else if (opt[4].getGlobalBounds().contains(mousePosF)) {
                        if (!p.weapon) {
                            currentMsg = "!! No weapon equipped";
                        } else if (p.weapon->isMaxLevel()) {
                            currentMsg = "!! " + p.weapon->name + " is already MAX level!";
                        } else {
                            int cost = p.weapon->upgradeCost();
                            if (p.wallet.spendMoney(cost)) {
                                p.weapon->upgrade();
                                currentMsg = "Upgraded " + p.weapon->name + " to Lv." + to_string(p.weapon->level) + "! (-" + to_string(cost) + "G)";
                                Bow* bow = dynamic_cast<Bow*>(p.weapon);
                                if (bow && bow->isFireArrowUnlocked() && !p.hasFireArrow) {
                                    p.hasFireArrow  = true; p.fireArrowAmmo = 5;
                                    currentMsg += "\n>> Fire Arrows UNLOCKED! (+5 ammo)";
                                }
                            } else {
                                currentMsg = "!! Not enough Gold (need " + to_string(p.weapon->upgradeCost()) + "G)";
                            }
                        }
                    }
                }
            }

            // อัปเดตข้อความตามสถานะล่าสุด
            string weaponInfo = p.weapon ? (p.weapon->name + " Lv." + to_string(p.weapon->level) + "/3" + (p.weapon->isMaxLevel() ? " [MAX]" : "  ->Upgrade: " + to_string(p.weapon->upgradeCost()) + "G")) : "No weapon";
            string fireInfo = p.hasFireArrow ? ("x" + to_string(p.fireArrowAmmo) + " ammo  ->Refill x3: 20G") : "Locked (need Bow Lv.3)";

            opt[0].setString("-> HP Potion        25G   (have: " + to_string(pot.invHP) + ")");
            opt[1].setString("-> ATK Potion       25G   (have: " + to_string(pot.invAtk) + ")");
            opt[2].setString("-> CRIT Potion      25G   (have: " + to_string(pot.invCrit) + ")");
            opt[3].setString("-> Fire Arrow       20G   " + fireInfo);
            opt[4].setString("-> Upgrade Weapon         " + weaponInfo);

            goldText.setString("Gold: " + to_string(p.wallet.balance) + "G" + (p.hasFireArrow ? "  |  Fire Arrows: " + to_string(p.fireArrowAmmo) : ""));
            msgText.setString(currentMsg);

            // เอฟเฟกต์เอาเมาส์ชี้ (Hover Effect)
            for(int i=0; i<5; i++){
                if(opt[i].getGlobalBounds().contains(mousePosF)){
                    opt[i].setFillColor(sf::Color::Cyan);
                    opt[i].setString(opt[i].getString() + "  <-- Click!"); 
                } else {
                    opt[i].setFillColor(sf::Color::White);
                }
            }

            // วาดทุกอย่างลงจอ
            window.clear(sf::Color(30, 30, 60));
            window.draw(box);
            window.draw(titleText);
            window.draw(goldText);
            for(int i=0; i<5; i++) window.draw(opt[i]);
            window.draw(msgText);
            window.draw(escText);
            window.display();
        }
    }
};
#endif