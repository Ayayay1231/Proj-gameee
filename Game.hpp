// Game.hpp
#ifndef GAME_HPP
#define GAME_HPP
#include "TileMap.hpp"
#include <iostream>
#include <vector>
#include <fstream> 
#include "Monster.h"
#include "Npc.h"
#include "Player.h"
#include "Potion.h"
#include "Combat.h"
#include "Weapon.h"
#include "Shop.h"        // ✅ เพิ่ม
#include "FirstPage.h" 

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
    Potion potion;             // ✅ invHP/invAtk/invCrit อยู่ใน Potion แล้ว
    int monsterCount = 0;
    NPCManager npcSys;
    std::string playerName = "Hero"; 

    int gameState = 0; 
    NPC* talkingTo = nullptr; 
    int currentDialogPage = 0; 

    sf::Font font;
    sf::RectangleShape dialogBox;
    sf::Text dialogName, dialogText;
    sf::Text saveNotif; 
    int saveNotifTimer = 0; 

    std::string fullMsg, currentMsg;
    int charIdx = 0;
    sf::Clock typeClock;

    sf::Text optA, optB;
    int currentOption = 0;

public:
    Game() {
        window.create(sf::VideoMode(1280, 720), "PROJECT");
        window.setFramerateLimit(60);
        if (!font.loadFromFile("Minecraft.ttf")) std::cout << "Error: Cannot load font\n";
        
        dialogBox.setSize({1000.f, 200.f});
        dialogBox.setFillColor(sf::Color(0, 0, 0, 200));
        dialogBox.setOutlineThickness(4.f);
        dialogBox.setOutlineColor(sf::Color::White);
        dialogBox.setPosition(140.f, 480.f); 

        dialogName.setFont(font); dialogName.setCharacterSize(26);
        dialogName.setFillColor(sf::Color::Yellow); dialogName.setPosition(160.f, 490.f);

        dialogText.setFont(font); dialogText.setCharacterSize(24);
        dialogText.setFillColor(sf::Color::White); dialogText.setPosition(160.f, 530.f);
        
        saveNotif.setFont(font); saveNotif.setCharacterSize(30);
        saveNotif.setFillColor(sf::Color::Green);
        saveNotif.setString("Game Saved!"); saveNotif.setPosition(20.f, 20.f);

        optA.setFont(font); optA.setCharacterSize(24); optA.setPosition(750.f, 530.f);
        optB.setFont(font); optB.setCharacterSize(24); optB.setPosition(750.f, 580.f);

        // NPC 1: ตัวประหลาด
        vector<string> elderMsg = {"Hello there !", "You got F.", "you stupid nigga!"};
        npcSys.spawnNPC("map_world.json","Mystery creature", elderMsg, "npc1.png", 500.f, 400.f, 0.5f, 0.5f);
        npcSys.setLastNPCChoice(
            "Ah yes", "Oh noo",
            "Wish Aha blesses you!", "Suck my dih now"
        );

        // NPC 2: ทหารยาม
        vector<string> guardDialog = {"Sawaddee kub pom ruhee na.","wannee pom ja son karn lia hee","especially my sister rerorerorero."};
        npcSys.spawnNPC("map_house.json","Guard", guardDialog, "monster1.png", 300.f, 200.f, 0.5f, 0.5f);
        npcSys.setLastNPCChoice(
            "Krub phee", "Mai aow a",
            "Dee mak nong rak", "Tai sa mung!"
        );

        // ✅ NPC 3: Merchant — เปิด Shop.h
        vector<string> merchantMsg = {
            "Howdy traveler!",
            "I have goods for sale. Interested?"
        };
        npcSys.spawnNPC("map_world.json", "Merchant", merchantMsg, "npc1.png", 700.f, 300.f, 0.5f, 0.5f);
        npcSys.setLastNPCChoice(
            "Show me your wares",
            "Maybe later",
            "",
            "Safe travels!"
        );

        if (!map.load("map_world.json")) std::cout << "Map error\n";
        if (!playerTexture.loadFromFile("player2.png")) std::cout << "Player error\n";
        
        player.setTexture(playerTexture);
        sf::FloatRect bounds = player.getLocalBounds();
        player.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        player.setPosition(600.f, 600.f); 

        fadeRect.setSize({1280, 720}); fadeRect.setFillColor(sf::Color(0, 0, 0, 0)); 
        rpgPlayer.maxHp = 100; rpgPlayer.hp = rpgPlayer.maxHp;
        rpgPlayer.wallet.balance = 100; 
        rpgPlayer.level = 1; rpgPlayer.exp = 0;
        rpgPlayer.weapon = WeaponFactory::selectWeapon(1);
    }

    void saveGame() {
        std::ofstream file("save.txt");
        if (file.is_open()) {
            file << playerName << "\n"
                 << currentMapName << "\n"
                 << player.getPosition().x << " " << player.getPosition().y << "\n"
                 << rpgPlayer.hp << " " << rpgPlayer.maxHp << "\n"
                 << rpgPlayer.level << " " << rpgPlayer.exp << "\n"
                 << rpgPlayer.wallet.balance << "\n"
                 // ✅ inventory จาก Potion
                 << potion.invHP << " " << potion.invAtk << " " << potion.invCrit << "\n"
                 << (rpgPlayer.weapon ? rpgPlayer.weapon->level : 1) << "\n"
                 << rpgPlayer.hasFireArrow << " " << rpgPlayer.fireArrowAmmo << "\n";
            file.close(); saveNotifTimer = 120; 
        }
    }

    bool loadGame() {
        std::ifstream file("save.txt");
        if (file.is_open()) {
            std::getline(file, playerName);
            std::getline(file, currentMapName);
            float px, py; file >> px >> py; player.setPosition(px, py);
            file >> rpgPlayer.hp >> rpgPlayer.maxHp
                 >> rpgPlayer.level >> rpgPlayer.exp
                 >> rpgPlayer.wallet.balance;
            // ✅ load inventory จาก Potion
            file >> potion.invHP >> potion.invAtk >> potion.invCrit;
            int wLv = 1; file >> wLv;
            if (rpgPlayer.weapon) rpgPlayer.weapon->level = wLv;
            file >> rpgPlayer.hasFireArrow >> rpgPlayer.fireArrowAmmo;
            file.close(); map.load(currentMapName); return true;
        } return false; 
    }

    void startCombat() {
        monsterCount++; Monster monster(monsterCount);
        monster.name   = "Monster #" + std::to_string(monsterCount);
        monster.maxHp  = 50 + (monsterCount * 10); monster.hp = monster.maxHp;
        monster.maxDmg = 5 + (monsterCount * 2);
        Summary summary;
        bool victory = Combat::start(rpgPlayer, monster, potion, summary, window);
        if (victory) {
            rpgPlayer.wallet.addMoney((rand() % 31) + 20);
            rpgPlayer.exp += (rand() % 100) + ((monster.maxHp / 10) + monster.maxDmg);
            while (rpgPlayer.exp >= rpgPlayer.getNextLevelExp()) {
                rpgPlayer.exp -= rpgPlayer.getNextLevelExp();
                rpgPlayer.level++; rpgPlayer.maxHp += 20;
                rpgPlayer.hp = rpgPlayer.maxHp; rpgPlayer.baseMaxDmg += 5;   
            }
        } else { window.close(); }
    }

    void update() {
        if (gameState == 0) { 
            if (isFading) {
                alpha += 8; 
                if (alpha >= 255) {
                    alpha = 255;
                    if (map.load(pendingMap)) { currentMapName = pendingMap; player.setPosition(300.f, 300.f); isFading = false; }
                }
            } else {
                if (alpha > 0) { alpha -= 8; if (alpha < 0) alpha = 0; }
                if (alpha < 100) { 
                    sf::Vector2f move(0, 0);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 4;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 4;

                    sf::FloatRect p = player.getGlobalBounds(); p.left += 5; p.top += 5; p.width -= 10; p.height -= 10;
                    if (!map.isSolid(p.left + move.x, p.top) && !map.isSolid(p.left + p.width + move.x, p.top + p.height)) player.move(move.x, 0);
                    if (!map.isSolid(p.left, p.top + move.y) && !map.isSolid(p.left + p.width, p.top + p.height + move.y)) player.move(0, move.y);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) startCombat();
            }
        } 
        else if (gameState == 1 || gameState == 3) {  // ✅ ลบ state 6 ออก
            if (charIdx < (int)fullMsg.length() && typeClock.getElapsedTime().asMilliseconds() > 25) {
                currentMsg += fullMsg[charIdx]; charIdx++; typeClock.restart();
            }
            dialogText.setString(currentMsg);
        }

        if (saveNotifTimer > 0) saveNotifTimer--;
        fadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    }

    void render() {
        window.clear();
        window.draw(map);
        npcSys.drawAll(window, currentMapName);
        window.draw(player); 

        if (gameState >= 1) {
            window.draw(dialogBox);
            window.draw(dialogName);
            window.draw(dialogText);
        }

        if (gameState == 2) {
            optA.setString("-> " + talkingTo->choiceA);
            optB.setString("-> " + talkingTo->choiceB);
            optA.setFillColor(currentOption == 0 ? sf::Color::Red : sf::Color::White);
            optB.setFillColor(currentOption == 1 ? sf::Color::Red : sf::Color::White);
            window.draw(optA); window.draw(optB);
        }

        if (saveNotifTimer > 0) window.draw(saveNotif);
        window.draw(fadeRect);
        window.display();
    }

    void run() {
        FirstPage menu(1280.f, 720.f);
        int choice = menu.run(window); 
        if (choice == 0) { window.close(); return; }

        bool skipNaming = false;
        if (choice == 2) {
            if (loadGame()) skipNaming = true; 
            else std::cout << "No Save File Found!\n";
        }
        
        if (!skipNaming) {
            sf::Text nameTitle("Enter Your Name", font, 50); nameTitle.setFillColor(sf::Color::Yellow);
            sf::FloatRect bounds = nameTitle.getLocalBounds(); nameTitle.setOrigin(bounds.width / 2.f, bounds.height / 2.f); nameTitle.setPosition(1280.f / 2.f, 200.f);
            sf::Text nameInput("", font, 60); nameInput.setFillColor(sf::Color::White);
            sf::Text confirmHint("Press ENTER to start", font, 30); confirmHint.setFillColor(sf::Color(150, 150, 150));
            bounds = confirmHint.getLocalBounds(); confirmHint.setOrigin(bounds.width / 2.f, bounds.height / 2.f); confirmHint.setPosition(1280.f / 2.f, 500.f);

            playerName = ""; bool isNaming = true;
            while (isNaming && window.isOpen()) {
                sf::Event e;
                while (window.pollEvent(e)) {
                    if (e.type == sf::Event::Closed) { window.close(); return; }
                    if (e.type == sf::Event::TextEntered) {
                        if (e.text.unicode == 8) { if (!playerName.empty()) playerName.pop_back(); }
                        else if (e.text.unicode == 13) { if (!playerName.empty()) isNaming = false; }
                        else if (e.text.unicode < 128 && e.text.unicode > 31 && playerName.length() < 12) playerName += static_cast<char>(e.text.unicode);
                    }
                }
                nameInput.setString(playerName + "_"); bounds = nameInput.getLocalBounds();
                nameInput.setOrigin(bounds.width / 2.f, bounds.height / 2.f); nameInput.setPosition(1280.f / 2.f, 350.f);
                window.clear(sf::Color(20, 20, 30)); window.draw(nameTitle); window.draw(nameInput);
                if (!playerName.empty()) window.draw(confirmHint); window.display();
            }
        }

        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();

                if (e.type == sf::Event::KeyPressed) {
                    if (e.key.code == sf::Keyboard::F5) saveGame();

                    // ─── State 0: กด E คุย ───────────────────────────────
                    if (gameState == 0 && e.key.code == sf::Keyboard::E) {
                        bool interactedWithNPC = false;
                        for (auto& npc : npcSys.list) {
                            if (npc.mapNames == currentMapName && npc.isPlayerNear(player.getGlobalBounds())) {
                                gameState = 1; talkingTo = &npc; currentDialogPage = 0; 
                                fullMsg = talkingTo->messages[currentDialogPage];
                                currentMsg = ""; charIdx = 0; dialogName.setString(talkingTo->name); typeClock.restart();
                                interactedWithNPC = true; break; 
                            }
                        }
                        if (!interactedWithNPC) {
                            sf::FloatRect warpCheck = player.getGlobalBounds(); warpCheck.left -= 25; warpCheck.top -= 25; warpCheck.width += 50; warpCheck.height += 50;
                            for (auto& w : map.warps) {
                                if (warpCheck.intersects(w.rect)) {
                                    if (!w.nextMap.empty() && !isFading && alpha == 0) {
                                        isFading = true; pendingMap = w.nextMap; break;
                                    }
                                }
                            }
                        }
                    }
                    // ─── State 1: คุยปกติ ────────────────────────────────
                    else if (gameState == 1 && e.key.code == sf::Keyboard::Space) {
                        if (charIdx < (int)fullMsg.length()) {
                            currentMsg = fullMsg; charIdx = fullMsg.length();
                        } else {
                            currentDialogPage++;
                            if (currentDialogPage < (int)talkingTo->messages.size()) {
                                fullMsg = talkingTo->messages[currentDialogPage];
                                currentMsg = ""; charIdx = 0; typeClock.restart();
                            } else {
                                if (talkingTo->hasChoice) {
                                    gameState = 2; currentOption = 0;
                                } else {
                                    gameState = 0; talkingTo = nullptr;
                                }
                            }
                        }
                    }
                    // ─── State 2: เลือกตอบ ───────────────────────────────
                    else if (gameState == 2) {
                        if (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::Up)   currentOption = 0;
                        if (e.key.code == sf::Keyboard::S || e.key.code == sf::Keyboard::Down) currentOption = 1;
                        if (e.key.code == sf::Keyboard::Space || e.key.code == sf::Keyboard::Enter) {
                            // ✅ Merchant → เปิด Shop.h โดยตรง
                            if (currentOption == 0 && talkingTo->name == "Merchant") {
                                Shop::open(rpgPlayer, potion, window, font);
                                gameState = 0; talkingTo = nullptr;
                            } else {
                                gameState = 3;
                                fullMsg = (currentOption == 0) ? talkingTo->replyA : talkingTo->replyB;
                                currentMsg = ""; charIdx = 0; typeClock.restart();
                            }
                        }
                    }
                    // ─── State 3: โชว์ผลตอบ ─────────────────────────────
                    else if (gameState == 3 && e.key.code == sf::Keyboard::Space) {
                        if (charIdx < (int)fullMsg.length()) {
                            currentMsg = fullMsg; charIdx = fullMsg.length();
                        } else {
                            gameState = 0; talkingTo = nullptr;
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