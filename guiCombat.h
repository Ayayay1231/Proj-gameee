#ifndef GUICOMBAT_H
#define GUICOMBAT_H

#include "unit.h"
#include <iostream>
#include<SFML/Graphics.hpp>
#define Max_Items 4

using namespace std;

class Gui 
{
    private:
    sf::Font btl;
    sf::Text sbtl[Max_Items];
    sf::RectangleShape atk;
    bool battle = false;
    int selectItem = 0;





public:
     Gui(float w, float h,bool battle);
     void Draws(sf::RenderWindow& window);


    

};

#endif GUICOMBAT_H