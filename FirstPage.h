// FirstPage.h
#ifndef FIRSTPAGE_H
#define FIRSTPAGE_H
#include <SFML/Graphics.hpp>

class FirstPage {
public:
    FirstPage(float width, float height);
    // ฟังก์ชัน run จะคืนค่า: 0=Exit, 1=New Game, 2=Load Game
    int run(sf::RenderWindow &window); 

private:
    sf::Font font;
    sf::Text title, playBtn, loadBtn, exitBtn, creditBtn;
    sf::Text creditList;
    void setupText(sf::Text &text, std::string str, float x, float y);

    sf::Texture bgTex;
    sf::Sprite bgSprite;
    bool hasBackground = false;
    bool showCredit = false;
};
#endif