#ifndef MONSTER_H
#define MONSTER_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
using namespace std;

class Monster {
public:
    string name;
    int hp, maxHp, maxDmg; // เพิ่ม maxHp เข้าไปตรงนี้
    sf::Texture texture;
    sf::Sprite sprite;


    Monster(int rank) {
        name = "Monster #" + to_string(rank);
        maxHp = 50 + (rank * 10);
        hp = maxHp;
        maxDmg = 5 + (rank * 2);

        int imageIndx =  1;
        string imgpath = "monster" + to_string(imageIndx) + ".png" ;

        if(!texture.loadFromFile(imgpath)){
            cout << "cannot load " << imgpath << endl;

        }else{
            sprite.setTexture(texture);
            sprite.setPosition(500.f,150.f);
            
        }
    }
};
#endif
