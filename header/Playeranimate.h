#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>

class Player {
private:
    // ต้องยกตัวแปรพวกนี้มาจาก .cpp มาไว้ที่นี่ครับ
    sf::Sprite player;
    sf::Texture texter;
    float movespd;
    float ttltime = 0;
    int currenttime = 0;

public:
    Player(std::string file);
    void upate(float time);
    void draw(sf::RenderWindow& window); // เพิ่มฟังก์ชันวาดด้วยครับ
    void setScale(float x, float y);
};

#endif
