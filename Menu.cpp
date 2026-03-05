#include<stdio.h>
#include<iostream>
#include <SFML/Graphics.hpp>
#include"Menu.h"

Menu::Menu(float w,  float h){
    if(!font.loadFromFile("CRA.ttf")){
        std::cout<<"Load failed" <<std::endl;
    }
    menu[0].setFont(font);
    menu[0].setCharacterSize(72);
    menu[0].setFillColor(sf::Color::Red);
    menu[0].setString("Play");
    menu[0].setOrigin(menu[0].getLocalBounds().width /2,menu[0].getLocalBounds().height/2);
    menu[0].setPosition(sf::Vector2f(w/2,h/(Max_Items+1)*1));

    menu[1].setFont(font);
    menu[1].setCharacterSize(72);
    menu[1].setFillColor(sf::Color::White);
    menu[1].setString("Hee");
    menu[1].setOrigin(menu[1].getLocalBounds().width /2,menu[1].getLocalBounds().height/2);
    menu[1].setPosition(sf::Vector2f(w/2,h/(Max_Items+1)*2));

    menu[2].setFont(font);
    menu[2].setCharacterSize(72);
    menu[2].setFillColor(sf::Color::White);
    menu[2].setString("Exit");
    menu[2].setOrigin(menu[2].getLocalBounds().width /2,menu[2].getLocalBounds().height/2);
    menu[2].setPosition(sf::Vector2f(w/2,h/(Max_Items+1)*3));
}
Menu::~Menu()
{

}

void Menu::draw(sf::RenderWindow& window)
{
    for(int i = 0; i<Max_Items;i++){
        window.draw(menu[i]);
    }
}
void Menu::MoveUp(){
    if(selectedItem-1 >= 0)
    {
        menu[selectedItem].setFillColor(sf::Color::White);
        selectedItem--;
        menu[selectedItem].setFillColor(sf::Color::Red);
    }
}
void Menu::MoveDown(){
    if(selectedItem+1 < Max_Items)
    {
        menu[selectedItem].setFillColor(sf::Color::White);
        selectedItem++;
        menu[selectedItem].setFillColor(sf::Color::Red);
    }
}