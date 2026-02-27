//มุกมิก
#include "FirstPage.h"
#include <iostream>

FirstPage::FirstPage(float width, float height) {
    if (!font.loadFromFile("../CRA.ttf")) { /* Handle Error */ }

    setupText(title, "MY GAME", width/2.0f, 100.0f);
    title.setCharacterSize(80); 
    title.setFillColor(sf::Color::Yellow);
    
    // --- ส่วนที่แก้: ต้องสั่งจัดกึ่งกลางอีกรอบ หลังจากขยายขนาดฟอนต์เป็น 80 ---
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    // -------------------------------------------------------------
    
    setupText(playBtn, "PLAY GAME", width/2.0f, 300.0f);
    setupText(settingsBtn, "SETTINGS", width/2.0f, 400.0f);
    setupText(exitBtn, "EXIT", width/2.0f, 500.0f);
}

void FirstPage::setupText(sf::Text &text, std::string str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::White);
    
    
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f); 
    text.setPosition(x, y);
}

int FirstPage::run(sf::RenderWindow &window) {
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return 0; // ปิดโปรแกรม

            
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (playBtn.getGlobalBounds().contains(mousePosF)) return 1; // เข้า
                if (settingsBtn.getGlobalBounds().contains(mousePosF)) return 2; // ตั้งค่า
                if (exitBtn.getGlobalBounds().contains(mousePosF)) return 0; // ออก
            }
        }

        
        playBtn.setFillColor(playBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
        settingsBtn.setFillColor(settingsBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
        exitBtn.setFillColor(exitBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);

        window.clear();
        window.draw(title);
        window.draw(playBtn);
        window.draw(settingsBtn);
        window.draw(exitBtn);
        window.display();
    }
    return 0;
}