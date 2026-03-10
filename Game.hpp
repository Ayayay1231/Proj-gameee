// Game.hpp - FULL VERSION (Fixed Braces & Shop System)
#ifndef GAME_HPP
#define GAME_HPP
#include "TileMap.hpp"
#include <iostream>
#include <vector>
#include <fstream> 
#include <cmath>
#include <string>
#include <SFML/Audio.hpp>

#include "shop.h"
#include "Npc.h"
#include "MapItem.h" 
#include "Player.h"
#include "Monster.h"
#include "Potion.h"
#include "Combat.h"
#include "Weapon.h"
#include "FirstPage.h" 
#include "scene.h"
#include "tribeEntity.h"

class Game {
private:
    sf::RenderWindow window;
    sf::View camera; 
    sf::View uiView; 
    TileMap map;
    sf::Sprite player;
    sf::Texture playerTexture; 
    // ใส่ไว้แถวๆ ที่ประกาศ player
    int playerDir = 0;    // 0=ลง, 1=ซ้าย, 2=ขวา, 3=ขึ้น (จำทิศล่าสุด)
    sf::Music bgMusic; 
    std::string currentBGM = "";

    sf::Clock animClock;  // อนิเม
    int currentFrame = 0; 
    int frameWidth = 66;  
    int frameHeight = 100; 
    int maxFrames = 4;   
     
    sf::RectangleShape fadeRect;
    float alpha = 0;          
    bool isFading = false;    
    std::string pendingMap = ""; 
    std::string currentMapName = "homie.json";

    Player rpgPlayer;
    Potion potion;
    int monsterCount = 0;
    
    NPCManager npcSys;
    MapItemManager itemSys; 
    Cutscene sceneMng;

    std::string playerName = "Hero"; 

    int gameState = 0; 
    NPC* talkingTo = nullptr; 
    int currentDialogPage = 0; 

    int currentMenuTab = 0; 
    int menuSelection = 0;  

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
    bool ePressedForWarp = false;

    std::vector<TribeEntity*> tribes; 
    sf::Texture tribeTex;
    sf::Sprite tribeSprite;
    sf::Clock deltaClock; // เอาไว้จับเวลา deltaTime
    bool gKeyWasPressed = false;

public:
    Game() {
        window.create(sf::VideoMode(1280, 720), "PlsNoFPlsNoF");
        window.setFramerateLimit(60);
        camera.setSize(1280.f, 720.f);
        camera.zoom(0.5f); // 🟢 ปรับความซูมตรงนี้ (0.5 คือซูมเข้า 2 เท่า ภาพจะเต็มจอกำลังสวย!)
        uiView.setSize(1280.f, 720.f);
        uiView.setCenter(1280.f / 2.f, 720.f / 2.f);
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
        saveNotif.setFillColor(sf::Color::Green); saveNotif.setPosition(20.f, 20.f);        

        optA.setFont(font); optA.setCharacterSize(24); optA.setPosition(750.f, 530.f);
        optB.setFont(font); optB.setCharacterSize(24); optB.setPosition(750.f, 580.f);

        // ของติดตัว
        Skill heavyStrike = {"Heavy Strike", "Deals 2.0x physical damage.", 10, 2.0f, 0};
        rpgPlayer.learnedSkills.push_back(heavyStrike);
        rpgPlayer.inventory.push_back({"Apple", "A fresh apple. Heals 20 HP.", 1, 20});

        rpgPlayer.inventory.push_back({"Wpn: Sword", "A balanced blade. [ENTER to Equip]", 1, 0});
        rpgPlayer.inventory.push_back({"Wpn: Baseball Bat", "Chance to STUN. [ENTER to Equip]", 1, 0});
        rpgPlayer.inventory.push_back({"Wpn: Bow", "Ranged. Can unlock Fire. [ENTER to Equip]", 1, 0});

        // สร้าง NPC (ID 1 คือคนแจกเควส)
        vector<string> gmMsg = {"Hello Hero..."}; 
        npcSys.spawnNPC(1, "church.json", "Guild Master", gmMsg, "1GM.png" , 303.f, 200.f, 0.08f, 0.08f);

        // สร้างชาวบ้านธรรมดา (ID 0)
        vector<string> Jane = {"Long time no see " + playerName + " !!", "There is a strange noise coming from the east, Did you hear that?"};
        vector<string> elderMsg = {"Hello there !", "You got F."};
        npcSys.spawnNPC(0, "village.json","Jane", Jane, "0Jane.png", 420.f, 185.f, 0.08f, 0.08f);

        // สร้างพ่อค้า (ID 2)
        vector<string> shopMsg = {""}; 
        npcSys.spawnNPC(2, "store.json", "Merchant", shopMsg, "2Sell.png", 270.f, 265.f, 0.08f, 0.08f);


        vector<string> boss = {""};
        npcSys.spawnEnemy("lastboss.json",99,"bosstrue.png",200.f,150.f,0.3f,0.3f);
        
        // สร้างมอนสเตอร์บนแมพ (ID 1 = สไลม์)
        npcSys.spawnEnemy("underground.json", 1, "Black_grouse_Flight.png", 200.f, 200.f, 0.5f, 0.5f);
        npcSys.spawnEnemy("underground.json", 1, "Black_grouse_Flight.png", 250.f, 200.f, 0.5f, 0.5f);
        npcSys.spawnEnemy("underground.json", 1, "Black_grouse_Flight.png",  230.f, 250.f, 0.5f, 0.5f);

        if (!map.load("homie.json")) std::cout << "Map error\n";
        if (!playerTexture.loadFromFile("playerani.png")) std::cout << "Player error\n";

        if (tribeTex.loadFromFile("Black_grouse_Flight.png")) {
            tribeSprite.setTexture(tribeTex);
            tribeSprite.setScale(0.1f, 0.1f); 
            // เซ็ตจุดศูนย์กลางให้อยู่ตรงกลางรูป
            tribeSprite.setOrigin(tribeTex.getSize().x / 2.f, tribeTex.getSize().y / 2.f);
        }

        // 🟢 สร้างชนเผ่า 2 ตัว (ตัวแรกเกิดมาคะแนน 50=Neutral, ตัวสอง 10=Hostile)
       // tribes.push_back(new TribeEntity(50.0f, sf::Vector2f(500.f, 300.f), "Black_grouse_Flight.png"));
        tribes.push_back( new TribeEntity(10.0f, sf::Vector2f(300.f, 400.f), "Black_grouse_Flight.png"));
        
        deltaClock.restart(); // เริ่มจับเวลาตอนเปิดเกม
        
        player.setTexture(playerTexture);
        sf::FloatRect bounds = player.getLocalBounds();
        player.setScale(0.3f,0.3f);// ==========================================

       // player.setOrigin(33.f,50.f);
        // 1. ระบบเคลื่อนที่ของ Player และ Animation
        // ==========================================
        sf::Vector2f mov(0, 0);
        float speed = 4.0f;
        int row = 0; // เก็บว่าตอนนี้เดินไปทิศไหน (เพื่อเลือกแถวในรูป Sprite Sheet)

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { mov.y += speed; row = 0; } // แถวที่ 0: เดินลง
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { mov.y -= speed; row = 3; } // แถวที่ 3: เดินขึ้น
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { mov.x -= speed; row = 1; } // แถวที่ 1: เดินซ้าย
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { mov.x += speed; row = 2; } // แถวที่ 2: เดินขวา

        if (mov.x != 0 || mov.y != 0) {
            // 🟢 ถ้ามีการกดเดิน ให้เช็คว่าเดินชนกำแพงไหม
            sf::Vector2f p = player.getPosition();
            if (!map.isSolid(p.x + mov.x, p.y + mov.y)) {
                player.move(mov);
            }

            // 🟢 ระบบเล่นแอนิเมชัน (สลับเฟรม)
            // เช็คว่าเวลาผ่านไป 0.15 วินาทีหรือยัง (150 ms)
            if (animClock.getElapsedTime().asMilliseconds() > 150) {
                currentFrame++; // ขยับไปเฟรมถัดไป
                if (currentFrame >= maxFrames) {
                    currentFrame = 0; // วนกลับมาเฟรมแรกใหม่
                }
                
                // ใช้กรอบ (IntRect) ไปตัดรูปภาพใน Texture ออกมาโชว์
                player.setTextureRect(sf::IntRect(currentFrame * frameWidth, row * frameHeight, frameWidth, frameHeight));
                
                animClock.restart(); // เริ่มจับเวลาเฟรมใหม่
            }
        } else {
            // 🟢 ถ้าไม่ได้กดปุ่มอะไรเลย ให้กลับมายืนตรง (เฟรมที่ 0)
            player.setTextureRect(sf::IntRect(0, row * frameHeight, frameWidth, frameHeight));
        }

        player.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        
        // 🟢 2. เซ็ตจุดศูนย์กลางจากขนาดของ "1 เฟรม" (ไม่ใช่ทั้งแผ่น)
        player.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
        
        // 3. วางพิกัดเกิด
        player.setPosition(378.f, 241.f);
        
        fadeRect.setSize({1280, 720}); fadeRect.setFillColor(sf::Color(0, 0, 0, 0)); 

        rpgPlayer.maxHp = 100; rpgPlayer.hp = rpgPlayer.maxHp; 
        rpgPlayer.maxMp = 50; rpgPlayer.mp = rpgPlayer.maxMp;
        rpgPlayer.wallet.balance = 100;
        rpgPlayer.level = 1; rpgPlayer.exp = 0; rpgPlayer.weapon = WeaponFactory::selectWeapon(1);
    } 
    // !!! จุดที่ 1: ลบปีกกาเกินที่ปิดคลาสก่อนกำหนดออกไปแล้ว !!!

    void updateBGM(std::string mapName) {
        std::string targetBGM = "";
        
        // ==========================================
        // 🎵 จัดกลุ่มเพลงให้แต่ละแมพ (แก้ชื่อไฟล์ .ogg ตามที่คุณมีได้เลย)
        // ==========================================
        if (mapName == "homie.json") {
            targetBGM = "bgm_home.ogg"; 
        }
        // 🟢 ถ้าเดินเข้าเมือง โบสถ์ หรือร้านค้า ใช้เพลงเดียวกัน (เดินข้ามแมพเพลงจะไม่เริ่มใหม่!)
        else if (mapName == "village.json" || mapName == "church.json" || mapName == "store.json") {
            targetBGM = "village.ogg";
        }
        // 🟢 ถ้าลงดันเจี้ยน
        else if (mapName == "abandon.json" || mapName == "tunnel.json" || mapName == "underground.json" || mapName == "lastboss.json") {
            targetBGM = "Dun.ogg";
        }
        // 🟢 แมพบอส
      //  else if (mapName == "underground.json" || mapName == "lastboss.json") {
        //    targetBGM = "bgm_boss.ogg";
        //}

        // ==========================================
        // 🛑 ระบบกันเพลงเริ่มใหม่: 
        // ถ้าแมพนี้ไม่มีเพลง หรือเป็นเพลงเดียวกับที่เล่นอยู่แล้ว ให้ข้ามไปเลย!
        // ==========================================
        if (targetBGM == "" || targetBGM == currentBGM) {
            return; 
        }

        // ถ้าเป็นเพลงใหม่ สั่งหยุดเพลงเก่าแล้วเปิดเพลงใหม่
        bgMusic.stop();
        if (bgMusic.openFromFile(targetBGM)) {
            bgMusic.setLoop(true);
            bgMusic.setVolume(30.f); // ปรับความดังตรงนี้ (0-100)
            bgMusic.play();
            currentBGM = targetBGM; // จำไว้ว่าตอนนี้กำลังเล่นเพลงนี้อยู่
        }
    }

    void saveGame() {
        std::ofstream file("save.txt");
        if (file.is_open()) {
            file << playerName << "\n" << currentMapName << "\n" << player.getPosition().x << " " << player.getPosition().y << "\n" << rpgPlayer.hp << " " << rpgPlayer.maxHp << "\n" << rpgPlayer.level << " " << rpgPlayer.exp << "\n" << rpgPlayer.wallet.balance << " " << rpgPlayer.invHP << "\n";
            file.close(); 
            saveNotif.setString("Game Saved!");
            saveNotifTimer = 5;
        }
    }

    bool loadGame() {
        std::ifstream file("save.txt");
        if (file.is_open()) {
            std::getline(file, playerName); std::getline(file, currentMapName);
            float px, py; file >> px >> py; player.setPosition(px, py);
            file >> rpgPlayer.hp >> rpgPlayer.maxHp >> rpgPlayer.level >> rpgPlayer.exp >> rpgPlayer.wallet.balance >> rpgPlayer.invHP;
            file.close(); map.load(currentMapName); return true;
        } return false; 
    }

    void startCombat(int mId) { 
        Monster monster(mId); 
        Summary summary; 
        bool victory = Combat::start(rpgPlayer, monster, potion, summary, window);
        if (victory) {
            rpgPlayer.wallet.addMoney((rand() % 31) + 20); 
            rpgPlayer.exp += (rand() % 100) + ((monster.maxHp / 10) + monster.maxDmg);
            
            // อัปเดตความคืบหน้าเควส
            if (mId == 1) { 
                rpgPlayer.slimesKilled++;
                for (auto& q : rpgPlayer.questLog) {
                    if (q.name == "Slay the Black grouses" && !q.isCompleted) {
                        q.progress = rpgPlayer.slimesKilled;
                        if (q.progress > q.maxProgress) q.progress = q.maxProgress; 
                        saveNotif.setString("Quest Updated: " + std::to_string(q.progress) + "/" + std::to_string(q.maxProgress));
                        saveNotifTimer = 120;
                    }
                }
            }

            while (rpgPlayer.exp >= rpgPlayer.getNextLevelExp()) {
                rpgPlayer.exp -= rpgPlayer.getNextLevelExp(); rpgPlayer.level++; 
                rpgPlayer.maxHp += 20; rpgPlayer.hp = rpgPlayer.maxHp; 
                rpgPlayer.maxMp += 10; rpgPlayer.mp = rpgPlayer.maxMp;
                rpgPlayer.baseMaxDmg += 5; 
            }
        } else { 

            rpgPlayer.hp = rpgPlayer.maxHp; // ฟื้นคืนชีพ เลือดเต็ม
            rpgPlayer.mp = rpgPlayer.maxMp; // มานาเต็ม
            
            // สั่งให้จอเฟดดำ แล้ววาร์ปกลับแมพเริ่มต้น (เช่น homie.json)
            isFading = true;
            pendingMap = "homie.json"; // อยากให้ไปเกิดแมพไหน เปลี่ยนชื่อตรงนี้ได้เลย
            alpha = 0;
            
            saveNotif.setString("You died! Respawned safely.");
            saveNotifTimer = 180;
        } 
    }


    

    void update() {
        sf::Vector2f camPos = player.getPosition();
        float viewW = camera.getSize().x; 
        float viewH = camera.getSize().y; 
        float mapPxW = map.mapW * map.tileW;
        float mapPxH = map.mapH * map.tileH;

        if (mapPxW > viewW) {
            if (camPos.x < viewW / 2.f) camPos.x = viewW / 2.f; 
            else if (camPos.x > mapPxW - viewW / 2.f) camPos.x = mapPxW - viewW / 2.f; 
        } else { camPos.x = mapPxW / 2.f; }

        if (mapPxH > viewH) {
            if (camPos.y < viewH / 2.f) camPos.y = viewH / 2.f; 
            else if (camPos.y > mapPxH - viewH / 2.f) camPos.y = mapPxH - viewH / 2.f; 
        } else { camPos.y = mapPxH / 2.f; }

        camera.setCenter(camPos); 



        // 🟢 1. ถ้าระบบคัตซีนทำงานอยู่ ให้อัปเดตตัวหนังสือแล้วหยุดทำงานอย่างอื่นเลย (ไม่ให้เดิน)
        if (sceneMng.isPlaying) {
            sceneMng.updateText();
            return;
        }

        // ถ้าเปิดเมนูหรือคุยอยู่ ให้จัดการแค่ข้อความแล้วหยุดทำงานส่วนอื่น
        if (gameState != 0) { 
            if (gameState == 1 || gameState == 3) { 
                if (charIdx < fullMsg.length() && typeClock.getElapsedTime().asMilliseconds() > 25) {
                    currentMsg += fullMsg[charIdx]; charIdx++; typeClock.restart();
                }
                dialogText.setString(currentMsg);
            }
            if (saveNotifTimer > 0) saveNotifTimer--;
            return; 
        }

        // ==========================================
        // 1. ระบบเคลื่อนที่ของ Player
        // ==========================================
        sf::Vector2f mov(0, 0);
        float speed = 2.15f;
        bool isMoving = false; 

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { mov.y += speed; playerDir = 0; isMoving = true; } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { mov.y -= speed; playerDir = 1; isMoving = true; } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { mov.x -= speed; playerDir = 2; isMoving = true; } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { mov.x += speed; playerDir = 3; isMoving = true; } 

        if (isMoving) { 
            sf::Vector2f p = player.getPosition();
            
            if (mov.x != 0 && !map.isSolid(p.x + mov.x, p.y)) {
                player.move(mov.x, 0);
            }
            if (mov.y != 0 && !map.isSolid(player.getPosition().x, p.y + mov.y)) {
                player.move(0, mov.y);
            }

            if (animClock.getElapsedTime().asMilliseconds() > 150) {
                currentFrame++; 
                if (currentFrame >= maxFrames) {
                    currentFrame = 0; 
                }
                player.setTextureRect(sf::IntRect(currentFrame * frameWidth, playerDir * frameHeight, frameWidth, frameHeight));
                animClock.restart(); 
            }
        } else {
            player.setTextureRect(sf::IntRect(0, playerDir * frameHeight, frameWidth, frameHeight));
            currentFrame = 0; 
        }


        // ==========================================
        // 🟢 ระบบอัปเดตชนเผ่า (Tribe AI)
        // ==========================================
        float dt = deltaClock.restart().asSeconds(); // คำนวณเวลาที่ผ่านไป (deltaTime)
        sf::Vector2f pPos = player.getPosition();

        bool gKeyIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::G);

        for (auto& tribe : tribes) {
            // 1. ให้ AI คำนวณการเดิน (เดินเข้าหา, หนี, หรือพุ่งเข้าใส่)
            tribe->update(dt, pPos);

            // 2. ระบบให้ของขวัญ (กดปุ่ม G เมื่ออยู่ใกล้ๆ)
            // เช็คระยะห่างระหว่างเรากับเขา
            
        }
        gKeyWasPressed = gKeyIsPressed; // อัปเดตสถานะปุ่ม

        // ==========================================
        // 2. ระบบเช็คการชนมอนสเตอร์
        // ==========================================
        sf::FloatRect hitBox = player.getGlobalBounds();
        
        for (auto it = npcSys.list.begin(); it != npcSys.list.end(); ) {
            if (it->mapNames == currentMapName && it->isEnemy) {
                
                // 🟢 1. ดักไว้! ถ้า "ไม่ใช่บอส" (ID ไม่ใช่ 99) ถึงจะวิ่งไล่และหั่นภาพ
                if (it->monsterId != 99) {
                    
                    sf::Vector2f ePos = it->sprite.getPosition();
                    float dx = pPos.x - ePos.x;
                    float dy = pPos.y - ePos.y;
                    float dist = std::sqrt(dx*dx + dy*dy); 

                    float aggroRange = 250.0f; 
                    float enemySpeed = 1.2f;   

                    if (dist < aggroRange && dist > 15.f) { 
                        float vx = (dx / dist) * enemySpeed;
                        float vy = (dy / dist) * enemySpeed;

                        if (!map.isSolid(ePos.x + vx, ePos.y)) ePos.x += vx;
                        if (!map.isSolid(ePos.x, ePos.y + vy)) ePos.y += vy;
                        it->sprite.setPosition(ePos);

                        int mDir = 0; 
                        if (std::abs(dx) > std::abs(dy)) mDir = (dx > 0) ? 2 : 1;
                        else mDir = (dy > 0) ? 0 : 3;

                        if (it->animClock.getElapsedTime().asMilliseconds() > 150) {
                            it->currentFrame++;
                            if (it->currentFrame >= 4) it->currentFrame = 0; 
                            
                            int mFrameWidth = 32; 
                            int mFrameHeight = 32;
                            it->sprite.setTextureRect(sf::IntRect(it->currentFrame * mFrameWidth, mDir * mFrameHeight, mFrameWidth, mFrameHeight));
                            
                            it->animClock.restart();
                        }
                    }
                } // 🟢 จบเงื่อนไขเช็คบอส

                // 🟢 3. โค้ดเช็คชนเพื่อเริ่มสู้ (บอสจะมาทำงานตรงนี้ได้ตามปกติ!)
                if (it->sprite.getGlobalBounds().intersects(hitBox)) {
                    int mId = it->monsterId;
                    it = npcSys.list.erase(it); // ลบตัวที่ชนออกจากฉาก
                    startCombat(mId);           // ตัดเข้าฉากสู้
                    return;                     
                } else { ++it; }
            } else { ++it; }
        }

        // ==========================================
        // 3. ระบบติดตามมุมกล้อง (Camera)
        // ==========================================
        

        // ==========================================
        // 4. ระบบวาร์ปเปลี่ยนแมพ
        // ==========================================
        bool standingOnWarp = false;
        std::string targetMap = "";

        for (auto& w : map.warps) {
            if (player.getGlobalBounds().intersects(w.rect)) {
                standingOnWarp = true;
                targetMap = w.nextMap;
                break;
            }
        }

        if (standingOnWarp && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if (!ePressedForWarp && !isFading) {
                isFading = true;
                pendingMap = targetMap;
                alpha = 0;
                ePressedForWarp = true; 
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            ePressedForWarp = false; 
        }

        // ==========================================
        // 5. ระบบ Fade และโหลดย้ายตำแหน่ง
        // ==========================================
        if (isFading) {
            alpha += 10;
            if (alpha >= 255) {
                alpha = 255;
                if (map.load(pendingMap)) {
                    std::string previousMap = currentMapName; 
                    currentMapName = pendingMap;             
                    
                    updateBGM(currentMapName);

                    if (pendingMap == "village.json") {
                        if (previousMap == "church.json") player.setPosition(551.0f, 278.0f); 
                        else if (previousMap == "homie.json") player.setPosition(154.0f, 382.0f); 
                        else if (previousMap == "store.json") player.setPosition(435.0f, 455.0f); 
                        else if (previousMap == "abandon.json") player.setPosition(627.0f, 285.0f); 
                        else player.setPosition(150.0f, 358.0f); 
                    }
                    else if (pendingMap == "homie.json") {
                        if (previousMap == "village.json") player.setPosition(323.0f, 244.0f); 
                        else player.setPosition(240.0f, 300.0f);
                    }
                    else if (pendingMap == "store.json") {
                        player.setPosition(277.0f, 373.0f);
                    }
                    else if (pendingMap == "abandon.json") {
                        if (previousMap == "tunnel.json") player.setPosition(342.0f, 358.0f); 
                        else if (previousMap == "underground.json") player.setPosition(342.0f, 376.0f); 
                        else if (previousMap == "village.json") player.setPosition(10.0f, 399.0f); 
                        else player.setPosition(10.0f, 399.0f);
                    }
                    else if (pendingMap == "church.json") {
                        player.setPosition(312.0f, 302.0f);
                    }
                    else if (pendingMap == "lastboss.json") {
                        player.setPosition(441.0f, 397.0f);
                    }
                    else if (pendingMap == "tunnel.json"){
                        if (previousMap == "lastboss.json") player.setPosition(314.0f, 26.0f);
                        else player.setPosition(310.0f, 444.0f); 
                    }
                    else if (pendingMap == "underground.json") {
                        if (previousMap == "abandon.json") player.setPosition(422.0f, 202.0f); 
                        else if (previousMap == "tunnel.json") player.setPosition(298.0f, 377.0f); 
                        else player.setPosition(422.0f, 202.0f); 
                    }
                }
                isFading = false;
            }
        } else if (alpha > 0) {
            alpha -= 10;
            if (alpha < 0) alpha = 0;
        }

        if (saveNotifTimer > 0) saveNotifTimer--;
        fadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    }











    void render() {
        // 🟢 2. ถ้าระบบคัตซีนทำงาน ให้วาดแค่คัตซีนเต็มจอ แล้วหยุดไม่ต้องวาดฉากเกมด้านล่างเลย
        if (sceneMng.isPlaying) {
            sceneMng.render(window, dialogBox, dialogName, dialogText);
            window.display();
            return; 
        }

        window.clear(); 

        window.setView(camera);
        window.draw(map); 
        itemSys.drawAll(window, currentMapName); 
        npcSys.drawAll(window, currentMapName); 
        for (auto tribe : tribes) {
          //  tribeSprite.setPosition(tribe.getPosition());
            window.draw(tribe->getSprite());

            
            // จัดตำแหน่งให้อยู่บนหัวพอดี
            
            
           // window.draw(stateText);
        }

        window.draw(player); // วาดผู้เล่นทีหลังสุด จะได้ทับตัวอื่น
        

        window.setView(uiView);
        
        if (gameState >= 1 && gameState <= 3) {
            window.draw(dialogBox); window.draw(dialogName); window.draw(dialogText);
            if (gameState == 2) {
                optA.setString("-> " + talkingTo->choiceA); optB.setString("-> " + talkingTo->choiceB);
                optA.setFillColor(currentOption == 0 ? sf::Color::Red : sf::Color::White);
                optB.setFillColor(currentOption == 1 ? sf::Color::Red : sf::Color::White);
                window.draw(optA); window.draw(optB);
            }
        }

        if (gameState == 5) {
            sf::RectangleShape menuBg({1000.f, 600.f}); menuBg.setFillColor(sf::Color(20, 20, 40, 240)); 
            menuBg.setOutlineThickness(4.f); menuBg.setOutlineColor(sf::Color::White); menuBg.setPosition(140.f, 60.f); window.draw(menuBg);

            sf::Text tabStatus("[ STATUS ]", font, 28); tabStatus.setPosition(170.f, 90.f);
            sf::Text tabInv("[ INVENTORY ]", font, 28); tabInv.setPosition(350.f, 90.f);
            sf::Text tabQuest("[ QUESTS ]", font, 28);  tabQuest.setPosition(580.f, 90.f);
            sf::Text tabSkill("[ SKILLS ]", font, 28);  tabSkill.setPosition(780.f, 90.f);
            
            tabStatus.setFillColor(currentMenuTab == 0 ? sf::Color::Yellow : sf::Color(100,100,100));
            tabInv.setFillColor(currentMenuTab == 1 ? sf::Color::Yellow : sf::Color(100,100,100));
            tabQuest.setFillColor(currentMenuTab == 2 ? sf::Color::Yellow : sf::Color(100,100,100));
            tabSkill.setFillColor(currentMenuTab == 3 ? sf::Color::Yellow : sf::Color(100,100,100));
            
            window.draw(tabStatus); window.draw(tabInv); window.draw(tabQuest); window.draw(tabSkill);

            sf::RectangleShape line({960.f, 2.f}); line.setFillColor(sf::Color::White); line.setPosition(160.f, 140.f); window.draw(line);

            int maxVisible = 10; 

            if (currentMenuTab == 0) {
                sf::Text sTitle("--- CHARACTER STATUS ---", font, 30);
                sTitle.setFillColor(sf::Color::Cyan); sTitle.setPosition(180.f, 170.f); window.draw(sTitle);
                
                // 🟢 1. ดึงพลังโจมตีและเลเวลของอาวุธที่กำลังถืออยู่มาคำนวณ
                int wDmg = rpgPlayer.weapon ? rpgPlayer.weapon->getDamage() : 0;
                int totalDmg = rpgPlayer.baseMaxDmg + wDmg;
                
                // 🟢 2. เอาชื่ออาวุธมาต่อท้ายด้วย (Lv. X) จะได้รู้ว่าอัปเกรดติดไหม
                string wName = rpgPlayer.weapon ? (rpgPlayer.weapon->name + " (Lv." + std::to_string(rpgPlayer.weapon->level) + ")") : "None";

                // 🟢 3. อัปเดตข้อความโชว์ให้มี Total Dmg (Base + Weapon)
                string statStr = "Name: " + playerName + 
                                 "\n\nLevel: " + std::to_string(rpgPlayer.level) + 
                                 "\nEXP: " + std::to_string(rpgPlayer.exp) + " / " + std::to_string(rpgPlayer.getNextLevelExp()) + 
                                 "\n\nHP: " + std::to_string(rpgPlayer.hp) + " / " + std::to_string(rpgPlayer.maxHp) + 
                                 "\nMP: " + std::to_string(rpgPlayer.mp) + " / " + std::to_string(rpgPlayer.maxMp) + 
                                 "\n\nTotal Dmg: " + std::to_string(totalDmg) + "  (Base " + std::to_string(rpgPlayer.baseMaxDmg) + " + Wpn " + std::to_string(wDmg) + ")" + 
                                 "\nWeapon: " + wName + 
                                 "\nCrit Chance: " + std::to_string(5 + (rpgPlayer.weapon ? rpgPlayer.weapon->getCritChance() : 0)) + "%\n\nGold: " + std::to_string(rpgPlayer.wallet.balance) + " G\n";
                
                sf::Text sText(statStr, font, 26); sText.setFillColor(sf::Color::White); sText.setPosition(180.f, 230.f); window.draw(sText);
            }
            else if (currentMenuTab == 1) { 
                int startIndex = 0;
                if (menuSelection >= maxVisible) startIndex = menuSelection - maxVisible + 1;
                int endIndex = std::min((int)rpgPlayer.inventory.size(), startIndex + maxVisible);

                for (int i = startIndex; i < endIndex; i++) {
                    sf::Text t(rpgPlayer.inventory[i].name + " (x" + std::to_string(rpgPlayer.inventory[i].amount) + ")", font, 26);
                    t.setPosition(180.f, 170.f + ((i - startIndex) * 40.f)); 
                    t.setFillColor(i == menuSelection ? sf::Color::Cyan : sf::Color::White);
                    if (i == menuSelection) t.setString("> " + t.getString());
                    window.draw(t);
                }
                if (rpgPlayer.inventory.size() > 0) {
                    sf::RectangleShape vLine({2.f, 450.f}); vLine.setFillColor(sf::Color(100, 100, 100)); vLine.setPosition(580.f, 150.f); window.draw(vLine);
                    sf::Text descTitle("[ ITEM INFO ]", font, 24); descTitle.setFillColor(sf::Color::Yellow); descTitle.setPosition(610.f, 170.f); window.draw(descTitle);
                    
                    sf::Text desc(rpgPlayer.inventory[menuSelection].desc, font, 24);
                    desc.setFillColor(sf::Color(200, 200, 200)); desc.setPosition(610.f, 220.f); window.draw(desc);
                }
            } 
            else if (currentMenuTab == 2) { 
                int startIndex = 0;
                if (menuSelection >= maxVisible) startIndex = menuSelection - maxVisible + 1;
                int endIndex = std::min((int)rpgPlayer.questLog.size(), startIndex + maxVisible);

                for (int i = startIndex; i < endIndex; i++) {
                    string status = rpgPlayer.questLog[i].isCompleted ? "[DONE] " : "[ACTIVE] ";
                    string prog = " (" + std::to_string(rpgPlayer.questLog[i].progress) + "/" + std::to_string(rpgPlayer.questLog[i].maxProgress) + ")";
                    if (rpgPlayer.questLog[i].maxProgress == 0) prog = ""; 

                    sf::Text t(status + rpgPlayer.questLog[i].name + prog, font, 26);
                    t.setPosition(180.f, 170.f + ((i - startIndex) * 40.f)); 
                    t.setFillColor(i == menuSelection ? sf::Color::Cyan : (rpgPlayer.questLog[i].isCompleted ? sf::Color(100,255,100) : sf::Color::White));
                    if (i == menuSelection) t.setString("> " + t.getString());
                    window.draw(t);
                }
                if (rpgPlayer.questLog.size() > 0) {
                    sf::RectangleShape vLine({2.f, 450.f}); vLine.setFillColor(sf::Color(100, 100, 100)); vLine.setPosition(580.f, 150.f); window.draw(vLine);
                    sf::Text descTitle("[ QUEST INFO ]", font, 24); descTitle.setFillColor(sf::Color::Yellow); descTitle.setPosition(610.f, 170.f); window.draw(descTitle);
                    
                    sf::Text desc(rpgPlayer.questLog[menuSelection].desc, font, 24);
                    desc.setFillColor(sf::Color(200, 200, 200)); desc.setPosition(610.f, 220.f); window.draw(desc);
                }
            } 
            else if (currentMenuTab == 3) { 
                int startIndex = 0;
                if (menuSelection >= maxVisible) startIndex = menuSelection - maxVisible + 1;
                int endIndex = std::min((int)rpgPlayer.learnedSkills.size(), startIndex + maxVisible);

                for (int i = startIndex; i < endIndex; i++) {
                    sf::Text t(rpgPlayer.learnedSkills[i].name, font, 26);
                    t.setPosition(180.f, 170.f + ((i - startIndex) * 40.f)); 
                    t.setFillColor(i == menuSelection ? sf::Color::Cyan : sf::Color::White);
                    if (i == menuSelection) t.setString("> " + t.getString());
                    if (i == rpgPlayer.equippedSkillIndex) { t.setString(t.getString() + "  [E]"); t.setFillColor(sf::Color::Green); }
                    window.draw(t);
                }
                if (rpgPlayer.learnedSkills.size() > 0) {
                    sf::RectangleShape vLine({2.f, 450.f}); vLine.setFillColor(sf::Color(100, 100, 100)); vLine.setPosition(580.f, 150.f); window.draw(vLine);
                    sf::Text descTitle("[ SKILL INFO ]", font, 24); descTitle.setFillColor(sf::Color::Yellow); descTitle.setPosition(610.f, 170.f); window.draw(descTitle);
                    
                    Skill selSkill = rpgPlayer.learnedSkills[menuSelection];
                    sf::Text desc("Cost: " + std::to_string(selSkill.mpCost) + " MP\n\n" + selSkill.desc, font, 24);
                    desc.setFillColor(sf::Color(200, 200, 200)); desc.setPosition(610.f, 220.f); window.draw(desc);
                }
            }
        }

        if (gameState == 6) {
            sf::RectangleShape confirmBg({550.f, 180.f});
            confirmBg.setFillColor(sf::Color(20, 20, 40, 240)); 
            confirmBg.setOutlineThickness(4.f); 
            confirmBg.setOutlineColor(sf::Color::Red); 
            confirmBg.setPosition(1280.f / 2.f - 275.f, 720.f / 2.f - 90.f);

            sf::Text warnText("Return to Main Menu?", font, 36);
            warnText.setFillColor(sf::Color::Yellow);
            sf::FloatRect bounds = warnText.getLocalBounds();
            warnText.setPosition(1280.f / 2.f - bounds.width / 2.f, 720.f / 2.f - 50.f);

            sf::Text guideText("[ SPACE ] Confirm      [ ESC ] Cancel", font, 24);
            guideText.setFillColor(sf::Color::White);
            bounds = guideText.getLocalBounds();
            guideText.setPosition(1280.f / 2.f - bounds.width / 2.f, 720.f / 2.f + 20.f);

            window.draw(confirmBg); window.draw(warnText); window.draw(guideText);
        }

        if (saveNotifTimer > 0) window.draw(saveNotif);
        window.draw(fadeRect); window.display();
    }

    void run() {
    START_MENU: 
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
            sf::FloatRect bounds = nameTitle.getLocalBounds(); nameTitle.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            nameTitle.setPosition(1280.f / 2.f, 200.f);
            sf::Text nameInput("", font, 60); nameInput.setFillColor(sf::Color::White);
            playerName = ""; bool isNaming = true;
            while (isNaming && window.isOpen()) {
                sf::Event e;
                while (window.pollEvent(e)) {
                    if (e.type == sf::Event::Closed) window.close();
                    if (e.type == sf::Event::TextEntered) {
                        if (e.text.unicode == 8) { if (!playerName.empty()) playerName.pop_back(); }
                        else if (e.text.unicode == 13 && !playerName.empty()) isNaming = false;
                        else if (e.text.unicode < 128 && e.text.unicode > 31 && playerName.length() < 12) playerName += (char)e.text.unicode;
                    }
                }
                nameInput.setString(playerName + "_"); 
                bounds = nameInput.getLocalBounds(); nameInput.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                nameInput.setPosition(1280.f / 2.f, 350.f);
                window.clear(sf::Color(20, 20, 30)); window.draw(nameTitle); window.draw(nameInput); window.display();
            }

            // (คัตซีนตอนตื่นนอนแบบใช้กล่องข้อความทับฉากของเก่า ยังทำงานได้ปกตินะครับ)
            gameState = 1;         
            talkingTo = nullptr;   
            fullMsg = "??? : " + playerName + "... Hey, " + playerName + "!\nWake up! Come outside quickly!"; 
            currentMsg = "";
            charIdx = 0;
            dialogName.setString("Unknown Voice"); 
            typeClock.restart();
            updateBGM(currentMapName);
        }

        bool returnToMenu = false; 

        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();
                
                // 🟢 3. ถ้าคัตซีนภาพทำงานอยู่ ให้มันรับปุ่มไปจัดการเองเลย
                if (sceneMng.isPlaying) {
                    sceneMng.handleInput(e);
                    continue; 
                }

                if (e.type == sf::Event::KeyPressed) {
                    
                    // 🟢 [ปุ่ม C ไว้เทสคัตซีน!] ลองกดได้ตอนเดินอยู่ในเกม
                    if (e.key.code == sf::Keyboard::C && gameState == 0) {
                        vector<string> imgs = {"cutscene1.png", "cutscene2.png"}; // ไปสร้างรูปชื่อนี้ไว้ด้วยนะ
                        vector<string> txts = {
                            "The monster has arrived... prepare yourself!",
                            "Press SPACEBAR to continue your journey."
                        };
                        sceneMng.start(imgs, txts);
                        continue;
                    }

                    if (e.key.code == sf::Keyboard::F5) saveGame();
                    
                    if (gameState == 0) { 
                        if (e.key.code == sf::Keyboard::Tab) { gameState = 5; currentMenuTab = 0; menuSelection = 0; }
                        else if (e.key.code == sf::Keyboard::Escape) { 
                            gameState = 6; 
                        }
                        
                        else if (e.key.code == sf::Keyboard::E) {
                            for (auto& npc : npcSys.list) {
                                if (npc.mapNames == currentMapName && npc.isPlayerNear(player.getGlobalBounds())) {
                                    if (npc.npcId == 2) {
                                        Shop::open(rpgPlayer, potion, window, font);
                                        break; 
                                    } 
                                    else if (npc.npcId == 1) { 
                                        bool hasQuest = false; bool isDone = false;
                                        for (auto& q : rpgPlayer.questLog) {
                                            if (q.name == "Slay the Black grouses") { hasQuest = true; isDone = q.isCompleted; break; }
                                        }

                                        if (!hasQuest) {
                                            npc.messages = {"Greetings, "+playerName , "There’s some eerie noise coming from the east.", "Can you go to check it?"};
                                            npc.setChoices("I will do it!", "No time.", "Thank you! Good luck.", "Come back if you change your mind.");
                                        } 
                                        else if (hasQuest && rpgPlayer.slimesKilled < 3) {
                                            npc.messages = {"You haven't killed 3 slimes yet!", "Go to the forest!"};
                                            npc.hasChoice = false; 
                                        } 
                                        else if (hasQuest && rpgPlayer.slimesKilled >= 3 && !isDone) {
                                            npc.messages = {"Incredible! You defeated the slimes!", "Here is your reward: 150 Gold and a Magic Potion!"};
                                            npc.hasChoice = false;
                                            
                                            rpgPlayer.wallet.addMoney(150);
                                            rpgPlayer.inventory.push_back({"Magic Potion", "Restores 50 HP", 1, 50});
                                            
                                            saveNotif.setString("Quest Completed! Got 150G & Potion!");
                                            saveNotifTimer = 180;

                                            for (auto& q : rpgPlayer.questLog) {
                                                if (q.name == "Slay the Black grouses") q.isCompleted = true;
                                            }
                                        } 
                                        else {
                                            npc.messages = {"Thanks for your help earlier!", "The village is safe."};
                                            npc.hasChoice = false;
                                        }
                                    }

                                    gameState = 1; talkingTo = &npc; currentDialogPage = 0; 
                                    fullMsg = talkingTo->messages[0]; currentMsg = ""; charIdx = 0; 
                                    dialogName.setString(talkingTo->name); typeClock.restart(); break;
                                }
                            }
                            for (auto& w : map.warps) {
                                if (player.getGlobalBounds().intersects(w.rect)) {
                                    isFading = true; pendingMap = w.nextMap; break;
                                }
                            }
                        }
                    }
                    else if (gameState == 6) {
                        if (e.key.code == sf::Keyboard::Escape) {
                            gameState = 0; 
                        } else if (e.key.code == sf::Keyboard::Space || e.key.code == sf::Keyboard::Enter) {
                            returnToMenu = true; 
                        }
                    }

                    else if (gameState == 1 && e.key.code == sf::Keyboard::Space) {
                        if (charIdx < fullMsg.length()) { currentMsg = fullMsg; charIdx = fullMsg.length(); }
                        else {
                            currentDialogPage++;
                            if (talkingTo != nullptr) {
                                if (currentDialogPage < talkingTo->messages.size()) {
                                    fullMsg = talkingTo->messages[currentDialogPage]; currentMsg = ""; charIdx = 0;
                                } else {
                                    if (talkingTo->hasChoice) { gameState = 2; currentOption = 0; }
                                    else { gameState = 0; }
                                }
                            } else {
                                gameState = 0;
                            }
                        }
                    }
                    else if (gameState == 2) {
                        if (e.key.code == sf::Keyboard::W) currentOption = 0;
                        if (e.key.code == sf::Keyboard::S) currentOption = 1;
                        if (e.key.code == sf::Keyboard::Space) {
                            gameState = 3; fullMsg = (currentOption == 0) ? talkingTo->replyA : talkingTo->replyB;
                            currentMsg = ""; charIdx = 0;
                        }
                    }
                    else if (gameState == 3 && e.key.code == sf::Keyboard::Space) { 
                        if (talkingTo && talkingTo->npcId == 1 && currentOption == 0) { 
                            bool hasQuest = false;
                            for (auto& q : rpgPlayer.questLog) { if (q.name == "Slay the Black grouses") hasQuest = true; }
                            
                            if (!hasQuest) { 
                                Quest newQuest = {"Slay the Black grouses", "Defeat 3 black grouses outside the village.", false, 0, 3};
                                rpgPlayer.questLog.push_back(newQuest);
                                saveNotif.setString("New Quest Accepted!");
                                saveNotifTimer = 120;
                            }
                        }
                        gameState = 0; talkingTo = nullptr; 
                    }
                    
                    else if (gameState == 5) { 
                        if (e.key.code == sf::Keyboard::Tab || e.key.code == sf::Keyboard::Escape) gameState = 0;
                        if (e.key.code == sf::Keyboard::A) { currentMenuTab = (currentMenuTab > 0) ? currentMenuTab - 1 : 0; menuSelection = 0; }
                        if (e.key.code == sf::Keyboard::D) { currentMenuTab = (currentMenuTab < 3) ? currentMenuTab + 1 : 3; menuSelection = 0; } 
                        if (currentMenuTab > 0) {
                            if (e.key.code == sf::Keyboard::W && menuSelection > 0) menuSelection--;
                            if (e.key.code == sf::Keyboard::S) {
                                int maxItems = 0;
                                if (currentMenuTab == 1) maxItems = rpgPlayer.inventory.size();
                                else if (currentMenuTab == 2) maxItems = rpgPlayer.questLog.size();
                                else if (currentMenuTab == 3) maxItems = rpgPlayer.learnedSkills.size();
                                if (maxItems > 0 && menuSelection < maxItems - 1) menuSelection++;
                            }
                        }
                        
                        if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::Space) {
                            if (currentMenuTab == 3) rpgPlayer.equippedSkillIndex = menuSelection;
                            else if (currentMenuTab == 1) { 
                                if (rpgPlayer.inventory.size() > 0) {
                                    string iName = rpgPlayer.inventory[menuSelection].name;

                                    if (iName == "Wpn: Sword") {
                                        rpgPlayer.weapon = rpgPlayer.ownedSword;
                                        saveNotif.setString("Equipped: Sword!"); saveNotifTimer = 120;
                                    }
                                    else if (iName == "Wpn: Baseball Bat") {
                                        rpgPlayer.weapon = rpgPlayer.ownedBat;
                                        saveNotif.setString("Equipped: Baseball Bat!"); saveNotifTimer = 120;
                                    }
                                    else if (iName == "Wpn: Bow") {
                                        rpgPlayer.weapon = rpgPlayer.ownedBow;
                                        saveNotif.setString("Equipped: Bow!"); saveNotifTimer = 120;
                                    }
                                    else if (rpgPlayer.inventory[menuSelection].healHp > 0 && rpgPlayer.inventory[menuSelection].amount > 0) {
                                        rpgPlayer.hp += rpgPlayer.inventory[menuSelection].healHp;
                                        if (rpgPlayer.hp > rpgPlayer.maxHp) rpgPlayer.hp = rpgPlayer.maxHp;
                                        
                                        rpgPlayer.inventory[menuSelection].amount--;
                                        if (rpgPlayer.inventory[menuSelection].amount <= 0) {
                                            rpgPlayer.inventory.erase(rpgPlayer.inventory.begin() + menuSelection);
                                            if (menuSelection > 0) menuSelection--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (returnToMenu) break; 
            
            update(); render();
        }

        if (returnToMenu) {
            gameState = 0;
            currentMapName = "homie.json";
            map.load(currentMapName);
            
            rpgPlayer = Player(); 
            rpgPlayer.inventory.clear();
            rpgPlayer.learnedSkills.clear();
            rpgPlayer.questLog.clear();
            
            Skill heavyStrike = {"Heavy Strike", "Deals 2.0x physical damage.", 10, 2.0f, 0};
            rpgPlayer.learnedSkills.push_back(heavyStrike);
            rpgPlayer.inventory.push_back({"Apple", "A fresh apple. Heals 20 HP.", 1, 20});
            rpgPlayer.inventory.push_back({"Wpn: Sword", "A balanced blade. [ENTER to Equip]", 1, 0});
            rpgPlayer.inventory.push_back({"Wpn: Baseball Bat", "Chance to STUN. [ENTER to Equip]", 1, 0});
            rpgPlayer.inventory.push_back({"Wpn: Bow", "Ranged. Can unlock Fire. [ENTER to Equip]", 1, 0});
            
            player.setPosition(378.f, 241.f);
            playerDir = 0; 
            
            npcSys.list.clear();

            for (auto t : tribes) delete t;
            tribes.clear();
            tribes.push_back(new TribeEntity(50.0f, sf::Vector2f(500.f, 300.f), "0Jane.png"));
            tribes.push_back(new TribeEntity(10.0f, sf::Vector2f(300.f, 400.f), "0Jane.png"));

            vector<string> gmMsg = {"Hello Hero..."};
          //  vector<string> gmMsg = {"Hello Hero..."}; 
            npcSys.spawnNPC(1, "church.json", "Guild Master", gmMsg, "1GM.png" , 303.f, 200.f, 0.08f, 0.08f);
            //vector<string> elderMsg = {"Hello there !", "You got F."};
            vector<string> Jane = {"Long time no see " + playerName + " !!", "There’s a strange noise coming from the east, Did you hear that?"};
            npcSys.spawnNPC(0, "village.json","Jane", Jane, "0Jane.png", 400.f, 185.f, 0.1f, 0.1f);
            vector<string> shopMsg = {""}; 
            npcSys.spawnNPC(2, "store.json", "Merchant", shopMsg, "2Sell.png", 270.f, 265.f, 0.08f, 0.08f);
            npcSys.spawnEnemy("lastboss.json",99,"bosstrue.png",200.f,150.f,0.3f,0.3f);
            npcSys.spawnEnemy("underground.json", 4, "Black_grouse.png", 200.f, 200.f, 0.2f, 0.2f);
            npcSys.spawnEnemy("underground.json", 4, "Black_grouse.png", 250.f, 200.f, 0.2f, 0.2f);
            npcSys.spawnEnemy("underground.json", 4, "Black_grouse.png",  230.f, 250.f, 0.2f, 0.2f);

            goto START_MENU;
        }
    }

    
};
#endif