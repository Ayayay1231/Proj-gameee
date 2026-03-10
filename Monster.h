#ifndef MONSTER_H
#define MONSTER_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
using namespace std;

class Monster {
public:
    string name;
    int hp, maxHp, maxDmg; 
    int bleedTurn = 0;
    int stunTurn = 0;
    int vulnTurn = 0;
    int burnTurn = 0;
    sf::Texture texture;
    sf::Sprite sprite;

    string imgpath = "";
    bool isBoss = false;       
    bool ultiUsed = false;     
    int ultiThreshold = 0;     
    string ultiName = "";      
    int ultiDmg = 0;           

    // 🟢 ตัวแปรสำหรับระบบ Zombie 🟢
    bool isZombie = false;
    int zombieState = 0;      // 0 = ปกติ, 1 = Infected, 2 = Rotten
    int zombieTimer = 0;      // นับเทิร์นสำหรับ Infected
    bool isFrenzied = false;  // โหมดคลั่ง (< 30% HP)

    Monster(int monsterId) {
       
        if (monsterId == 1) {
            name = "Black grouse"; maxHp = 30; maxDmg = 5; imgpath = "Black_grouse.png";
        } 
        else if (monsterId == 2) {
            name = "Goblin"; maxHp = 60; maxDmg = 12; imgpath = "monster2.png";
        } 
        else if (monsterId == 3) {
            name = "Orc Warrior"; maxHp = 120; maxDmg = 20; imgpath = "monster3.png";
        }
        // ==========================================
        // 🟢 เพิ่มซอมบี้ (ID 4) พร้อมตั้งค่าสถานะเริ่มต้น
        // ==========================================
        else if (monsterId == 4) {
            name = "Zombie Black grouse";
            maxHp = 200;
            maxDmg = 15;
            imgpath = "Black_grouse.png"; // เปลี่ยนรูปซอมบี้ตรงนี้ได้เลยครับ
            isZombie = true;
            zombieState = 1; // เริ่มมาติด Infected ทันที
            zombieTimer = 3; // นับเวลาเน่า 3 เทิร์น
        }
        else if (monsterId == 99) {
            name = "Aymie the devil pig"; maxHp = 500; maxDmg = 45; imgpath = "bosstrue.png";
            isBoss = true; ultiThreshold = 250; ultiName = "HELLFIRE BLAST"; ultiDmg = 50;
        }
        else {
            name = "Missingno"; maxHp = 10; maxDmg = 1; imgpath = "monster1.png";
        }

        hp = maxHp; 

        if(!texture.loadFromFile(imgpath)){
            cout << "cannot load " << imgpath << endl;
        }else{
            sprite.setTexture(texture);
            float targetWidth = 400.f;  
            float targetHeight = 400.f; 
            float origWidth = texture.getSize().x;
            float origHeight = texture.getSize().y;
            sprite.setScale(targetWidth / origWidth, targetHeight / origHeight);
            sprite.setPosition(400.f, 150.f); 
        }
    } 
};    
#endif