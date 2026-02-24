// Game.hpp
#ifndef GAME_HPP
#define GAME_HPP
#include "TileMap.hpp"
#include <iostream>
#include <vector>

#include "Npc.h"
#include "Player.h"
#include "Monster.h"
#include "Potion.h"
#include "Combat.h"
#include "Weapon.h"

class Game {
private:
    sf::RenderWindow window;
    TileMap map;
    
    sf::Sprite player;
    sf::Texture playerTexture; 
    
    sf::RectangleShape fadeRect;
    float alpha = 0;          
    bool isFading = false;    
    std::string pendingMap = ""; 
    std::string currentMapName = "map_world.json";

    Player rpgPlayer;
    Potion potion;
    int monsterCount = 0;
    
    NPCManager npcSys;

    int gameState = 0; 
    NPC* talkingTo = nullptr; 
    int currentDialogPage = 0; 

    sf::Font font;
    sf::RectangleShape dialogBox;
    sf::Text dialogName;
    sf::Text dialogText;

    std::string fullMsg;
    std::string currentMsg;
    int charIdx = 0;
    sf::Clock typeClock;

public:
    Game() {
        window.create(sf::VideoMode(1280, 720), "RPG Game - Unified System");
        window.setFramerateLimit(60);
        
        if (!font.loadFromFile("Minecraft.ttf")) { 
            std::cout << "Error: Cannot load font\n";
        }
        dialogBox.setSize({1000.f, 200.f});
        dialogBox.setFillColor(sf::Color(0, 0, 0, 200));
        dialogBox.setOutlineThickness(4.f);
        dialogBox.setOutlineColor(sf::Color::White);
        dialogBox.setPosition(140.f, 480.f); 

        dialogName.setFont(font);
        dialogName.setCharacterSize(26);
        dialogName.setFillColor(sf::Color::Yellow);
        dialogName.setPosition(160.f, 490.f);

        dialogText.setFont(font);
        dialogText.setCharacterSize(24);
        dialogText.setFillColor(sf::Color::White);
        dialogText.setPosition(160.f, 530.f);

        vector<string> elderMsg = {"Hello there !", "You got F.", "you stupid nigga!"};
        npcSys.spawnNPC("map_world.json","Mystery creature", elderMsg, "npc1.png", 500.f, 400.f, 0.5f, 0.5f);

        vector<string> guardDialog = {"Sawaddee kub pom ruhee na.","wannee pom ja son karn lia hee","especially my sister rerorerorero."};
        npcSys.spawnNPC("map_house.json","Guard", guardDialog, "monster1.png", 300.f, 200.f,0.5f,0.5f);

        if (!map.load("map_world.json")) {
            std::cout << "CRITICAL ERROR: Failed to load map_world.json! Check file path." << std::endl;
        } else {
            std::cout << "SUCCESS: Map loaded correctly." << std::endl;
        }
        
        if (!playerTexture.loadFromFile("player2.png")) {
            std::cout << "Error: Could not load player2.png" << std::endl;
        }
        player.setTexture(playerTexture);
        sf::FloatRect bounds = player.getLocalBounds();
        player.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        player.setPosition(600.f, 600.f); 

        fadeRect.setSize({1280, 720}); 
        fadeRect.setFillColor(sf::Color(0, 0, 0, 0)); 

        rpgPlayer.maxHp = 100;
        rpgPlayer.hp = rpgPlayer.maxHp;
        rpgPlayer.wallet.balance = 100; 
        rpgPlayer.level = 1;
        rpgPlayer.exp = 0;
        rpgPlayer.weapon = WeaponFactory::selectWeapon(1);
    }

    void startCombat() {
        monsterCount++;
        Monster monster(monsterCount); 
        monster.name = "Monster #" + std::to_string(monsterCount);
        monster.maxHp = 50 + (monsterCount * 10);
        monster.hp = monster.maxHp;
        monster.maxDmg = 5 + (monsterCount * 2);

        Summary summary;
        std::cout << "\n>>> Adventure #" << monsterCount << " begins!" << std::endl;
        
        bool victory = Combat::start(rpgPlayer, monster, potion, summary, window);

        if (victory) {
            std::cout << "VICTORY! Defeated " << monster.name << std::endl;
            int goldGained = (rand() % 31) + 20; 
            rpgPlayer.wallet.addMoney(goldGained);
            int gainedExp = (rand() % 100) + ((monster.maxHp / 10) + monster.maxDmg);
            rpgPlayer.exp += gainedExp;
            
            while (rpgPlayer.exp >= rpgPlayer.getNextLevelExp()) {
                rpgPlayer.exp -= rpgPlayer.getNextLevelExp();
                rpgPlayer.level++;
                rpgPlayer.maxHp += 20; 
                rpgPlayer.hp = rpgPlayer.maxHp; 
                rpgPlayer.baseMaxDmg += 5;   
                std::cout << "\n  [!] LEVEL UP! Current Level: " << rpgPlayer.level << std::endl;
            }
        } else {
            std::cout << "\n>>>> GAME OVER <<<<" << std::endl;
            window.close(); 
        }
    }

    void update() {
        if (gameState == 0) { 
            if (isFading) {
                alpha += 8; 
                if (alpha >= 255) {
                    alpha = 255;
                    if (map.load(pendingMap)) {
                        std::cout << "Warped to: " << pendingMap << std::endl;
                        
                        // <--- แก้บั๊ก 1: อัปเดตชื่อแมพปัจจุบันตอนวาร์ปเสร็จ
                        currentMapName = pendingMap; 
                        
                        player.setPosition(300.f, 300.f); 
                        isFading = false; 
                    }
                }
            } else {
                if (alpha > 0) {
                    alpha -= 8; 
                    if (alpha < 0) alpha = 0;
                }

                if (alpha < 100) { 
                    sf::Vector2f move(0, 0);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 4;

                    sf::FloatRect p = player.getGlobalBounds();
                    p.left += 5; p.top += 5; p.width -= 10; p.height -= 10;

                    if (!map.isSolid(p.left + move.x, p.top) && !map.isSolid(p.left + p.width + move.x, p.top + p.height))
                        player.move(move.x, 0);
                    if (!map.isSolid(p.left, p.top + move.y) && !map.isSolid(p.left + p.width, p.top + p.height + move.y))
                        player.move(0, move.y);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
                    startCombat();
                }
            }
        } 
        else if (gameState == 1) { 
            if (charIdx < fullMsg.length() && typeClock.getElapsedTime().asMilliseconds() > 25) {
                currentMsg += fullMsg[charIdx];
                charIdx++;
                typeClock.restart();
            }
            dialogText.setString(currentMsg);
        }

        fadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    }

    void render() {
        window.clear(); 
        window.draw(map); 
        
        // <--- แก้บั๊ก 2: ลบ if(map.load) ทิ้งไปเลย ไม่งั้นเกมจะแลคหนักมาก!
        npcSys.drawAll(window, currentMapName);
        
        window.draw(player); 
        
        if (gameState == 1) {
            window.draw(dialogBox);
            window.draw(dialogName);
            window.draw(dialogText);
        }

        window.draw(fadeRect); 
        window.display();
    }

    void run() {
        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();

                if (e.type == sf::Event::KeyPressed) {
                    
                    if (gameState == 0 && e.key.code == sf::Keyboard::E) {
                        bool interactedWithNPC = false;
                        
                        for (auto& npc : npcSys.list) {
                            // <--- แก้บั๊ก 3: ต้องเช็คด้วยว่า NPC อยู่แมพเดียวกับเราไหม ไม่งั้นกดคุยทะลุแมพ!
                            if (npc.mapNames == currentMapName && npc.isPlayerNear(player.getGlobalBounds())) {
                                gameState = 1;
                                talkingTo = &npc;
                                currentDialogPage = 0; 
                                fullMsg = talkingTo->messages[currentDialogPage];
                                currentMsg = "";
                                charIdx = 0;
                                dialogName.setString(talkingTo->name);
                                typeClock.restart();
                                interactedWithNPC = true;
                                break; 
                            }
                        }

                        if (!interactedWithNPC) {
                            sf::FloatRect warpCheck = player.getGlobalBounds();
                            warpCheck.left -= 25; warpCheck.top -= 25; warpCheck.width += 50; warpCheck.height += 50;

                            for (auto& w : map.warps) {
                                if (warpCheck.intersects(w.rect)) {
                                    if (!w.nextMap.empty() && !isFading && alpha == 0) {
                                        isFading = true;       
                                        pendingMap = w.nextMap;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if (gameState == 1 && e.key.code == sf::Keyboard::Space) {
                        if (charIdx < fullMsg.length()) {
                            currentMsg = fullMsg;
                            charIdx = fullMsg.length();
                        } else {
                            currentDialogPage++;
                            if (currentDialogPage < talkingTo->messages.size()) {
                                fullMsg = talkingTo->messages[currentDialogPage];
                                currentMsg = "";
                                charIdx = 0;
                                typeClock.restart();
                            } else {
                                gameState = 0;
                                talkingTo = nullptr;
                            }
                        }
                    }
                }
            }
            update(); 
            render();
        }
    }
};
#endif
