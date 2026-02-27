#include<stdio.h>
#include<iostream>
#include <SFML/Graphics.hpp>
#define Max_Items 3 
class Menu
{
    public:
    Menu(float w, float h);
    ~Menu();

    void draw(sf::RenderWindow& window);
    void MoveUp();
    void MoveDown();

    private:
    int selectedItem=0;
    sf::Font font;
    sf::Text menu[Max_Items];

};