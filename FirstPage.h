//มุกมิก
#ifndef FIRSTPAGE_H
#define FIRSTPAGE_H
#include <SFML/Graphics.hpp>

class FirstPage {
public:
    FirstPage(float width, float height);
    // ฟังก์ชัน run จะคืนค่า: 0=Exit, 1=Play, 2=Settings
    int run(sf::RenderWindow &window); 

private:
    sf::Font font;
    sf::Text title, playBtn, settingsBtn, exitBtn;
    void setupText(sf::Text &text, std::string str, float x, float y);
};
#endif