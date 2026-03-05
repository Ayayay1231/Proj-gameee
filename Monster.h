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
        bool isBoss = false;       // ตัวนี้เป็นบอสไหม?
        bool ultiUsed = false;     // ใช้ท่าไม้ตายไปหรือยัง? (ป้องกันการสแปม)
        int ultiThreshold = 0;     // เลือดต้องต่ำกว่าเท่าไหร่ถึงจะใช้ท่านี้
        string ultiName = "";      // ชื่อท่าไม้ตาย
        int ultiDmg = 0;           // ดาเมจของท่าไม้ตาย


    // เปลี่ยนจากรับ (int rank) มาเป็นรับ (int monsterId) แทน
    Monster(int monsterId) {
       
        // ==========================================
        // สร้างข้อมูลมอนสเตอร์เตรียมไว้กี่ตัวก็ได้!
        // ==========================================
        if (monsterId == 1) {
            name = "Slime";
            maxHp = 30;
            maxDmg = 5;
            imgpath = "monster1.png";
        } 
        else if (monsterId == 2) {
            name = "Goblin";
            maxHp = 60;
            maxDmg = 12;
            imgpath = "monster2.png";
        } 
        else if (monsterId == 3) {
            name = "Orc Warrior";
            maxHp = 120;
            maxDmg = 20;
            imgpath = "monster3.png";
        }
        else if (monsterId == 99) {
            // เผื่อทำบอสลับ!
            name = "Aymie the devil pig";
            maxHp = 500;
            maxDmg = 45;
            imgpath = "bosstrue.png";

            isBoss = true;
            ultiThreshold = 250;     // ถ้าเลือดลดเหลือ 250 (ครึ่งหลอด) จะคลั่ง!
            ultiName = "HELLFIRE BLAST"; 
            ultiDmg = 50;
        }
        else {
            // ถ้าใส่เลขผิด ให้เป็นตัวนี้แทน
            name = "Missingno";
            maxHp = 10; maxDmg = 1;
            imgpath = "monster1.png";
        }

        hp = maxHp; 

        if(!texture.loadFromFile(imgpath)){
            cout << "cannot load " << imgpath << endl;
        }else{
            sprite.setTexture(texture);
            
            // ==========================================
            // ปรับขนาดรูปมอนสเตอร์ "ในฉากสู้" ให้เท่ากันทุกตัว!
            // ==========================================
            float targetWidth = 400.f;  // ความกว้างที่อยากได้ในฉากสู้ (พิกเซล)
            float targetHeight = 400.f; // ความสูงที่อยากได้ในฉากสู้ (พิกเซล)

            // ดึงขนาดไฟล์รูปต้นฉบับ
            float origWidth = texture.getSize().x;
            float origHeight = texture.getSize().y;

            // สั่งคำนวณและปรับสเกล
            sprite.setScale(targetWidth / origWidth, targetHeight / origHeight);
            
            // จัดตำแหน่งให้อยู่ทางขวาของจอพอดีๆ ในฉากสู้
            sprite.setPosition(400.f, 150.f); 
        }
    } // ปิด Constructor
};    // ปิด Class
#endif