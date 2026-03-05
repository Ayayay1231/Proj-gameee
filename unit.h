#ifndef UNIT_H
#define UNIT_H
#include<SFML/Graphics.hpp>
#include <string>

class Unit {
    sf::Texture texture;
    sf::Sprite hero;
public:
    std::string name;
    int hp, maxHp, mp, atk, items;
    bool isGuarding;

    sf::Sprite sprite;

   Unit(std::string n, int h, int m, int a, int i,std::string image); // Constructor
    virtual void takeDamage(int dmg);// เอาไปใช้ในheadอื่นได้
    void draw(sf::RenderWindow& window);

    //void takeDamage(int damage);
    void heal(int amount);
    bool useSkill(Unit& target); // คืนค่า true ถ้าใช้สำเร็จ
    void showStats() const;
};

#endif