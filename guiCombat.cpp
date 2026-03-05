#include<iostream>
#include"guiCombat.h"
#include<SFML/Graphics.hpp>
#include"unit.h"

Gui::Gui(float w, float h,bool battle){
     if(!btl.loadFromFile("CRA.ttf")){
        std::cout<<"Load failed" <<std::endl;
    }
    sbtl[0].setFont(btl);
    sbtl[0].setCharacterSize(50);
    sbtl[0].setFillColor(sf::Color::White);
    sbtl[0].setString("ATTACK");
    sbtl[0].setOrigin(sbtl[0].getLocalBounds().width/2 , sbtl[0].getLocalBounds().height/2);
    sbtl[0].setPosition(sf::Vector2f(w/2 , h/(Max_Items+1)*1));

    sbtl[1].setFont(btl);
    sbtl[1].setCharacterSize(50);
    sbtl[1].setFillColor(sf::Color::White);
    sbtl[1].setString("GUARD");
    sbtl[1].setOrigin(sbtl[1].getLocalBounds().width/2 , sbtl[0].getLocalBounds().height/2);
    sbtl[1].setPosition(sf::Vector2f(w*0.75 , h/(Max_Items+1)*1));

    sbtl[2].setFont(btl);
    sbtl[2].setCharacterSize(50);
    sbtl[2].setFillColor(sf::Color::White);
    sbtl[2].setString("SKILLS");
    sbtl[2].setOrigin(sbtl[2].getLocalBounds().width/2 , sbtl[0].getLocalBounds().height/2);
    sbtl[2].setPosition(sf::Vector2f(w/2 , h/(Max_Items+1)*2));

    sbtl[3].setFont(btl);
    sbtl[3].setCharacterSize(50);
    sbtl[3].setFillColor(sf::Color::White);
    sbtl[3].setString("ESCAPE");
    sbtl[3].setOrigin(sbtl[3].getLocalBounds().width/2 , sbtl[0].getLocalBounds().height/2);
    sbtl[3].setPosition(sf::Vector2f(w*0.75 , h/(Max_Items+1)*2));
    
}

