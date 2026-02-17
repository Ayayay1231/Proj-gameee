#include "Player.h"
#include <iostream>

// ไม่ต้องใส่ class Player { อีกแล้ว เพราะมันซ้ำซ้อน

Player::Player(std::string file) {
    if (!texter.loadFromFile(file)) {
        std::cout << "cannot load " << file << std::endl;
    }
    player.setTexture(texter);
    player.setTextureRect(sf::IntRect(4, 56, 36, 48));
    movespd = 300.f;
    ttltime = 0;   // กำหนดค่าเริ่มต้นตรงนี้ชัวร์กว่า
    currenttime = 0;
}

void Player::upate(float time) {
    bool  isMoving= false;

    // เช็คทิศทาง
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { player.move(0, -1.f); isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { player.move(0, 1.f);  isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { player.move(-1.f, 0); isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
         player.move(1.f, 0);  
         isMoving = true; 
    }
    // เล่น Animation เมื่อมีการเคลื่อนที่
     if (isMoving) {
        ttltime += time;
        if (ttltime >= 0.05f) {
            ttltime = 0;
            currenttime = (currenttime + 1) % 6; // วนลูป 6 เฟรม
            
            int newX = 4 + (currenttime * 40); // 40 คือระยะห่างระหว่างเฟรม (ต้องกะให้ตรงกับรูป)
            
            // *** จุดที่แก้: ต้องใช้ setTextureRect และครอบด้วย sf::IntRect ***
            player.setTextureRect(sf::IntRect(newX, 56, 40, 48));
        }
    } else {
        // ถ้าไม่เดิน ให้กลับไปเฟรมยืนนิ่ง (เฟรม 0)
        ttltime += time;
        if(ttltime >=0.2f){
            ttltime = 0;

            currenttime = (currenttime + 1) % 7; // วนลูป 6 เฟรม
            
            int newX = 0 + (currenttime * 40); // 40 คือระยะห่างระหว่างเฟรม (ต้องกะให้ตรงกับรูป)
            
            // *** จุดที่แก้: ต้องใช้ setTextureRect และครอบด้วย sf::IntRect ***
            player.setTextureRect(sf::IntRect(newX, 5, 40, 45));
        }
    }
}
void Player::draw(sf::RenderWindow& window) {
    window.draw(player);
}
void Player::setScale(float x, float y) {
    player.setScale(x, y); // ไปสั่ง Sprite ข้างในคลาสอีกที
}
