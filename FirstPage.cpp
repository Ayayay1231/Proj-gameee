// FirstPage.cpp
#include "FirstPage.h"
#include <iostream>

FirstPage::FirstPage(float width, float height) {
    if (!font.loadFromFile("Minecraft.ttf")) { 
        std::cout << "Error: Cannot load Minecraft.ttf\n";

        
    }
    if (bgTex.loadFromFile("bggame.png")) {
        bgSprite.setTexture(bgTex);
        // ปรับขนาดภาพให้เต็มจอ 1280x720
        bgSprite.setScale(
            width / bgTex.getSize().x, 
            height / bgTex.getSize().y
        );
        hasBackground = true;
    }

    setupText(title, "KORACH", width/2.0f, 100.0f);
    title.setCharacterSize(80); 
    title.setOutlineThickness(5.f);
    title.setFillColor(sf::Color::Yellow);
    title.setOutlineColor(sf::Color::Black);
    
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    
    setupText(playBtn, "NEW GAME", width/2.0f, 300.0f);
    playBtn.setOutlineThickness(5.f);
    playBtn.setOutlineColor(sf::Color::Black);
    setupText(loadBtn, "LOAD GAME", width/2.0f, 400.0f); // <--- เปลี่ยนเป็น LOAD GAME
    loadBtn.setOutlineThickness(5.f);
    loadBtn.setOutlineColor(sf::Color::Black);
    setupText(exitBtn, "EXIT", width/2.0f, 600.0f);
    exitBtn.setOutlineThickness(5.f);
    exitBtn.setOutlineColor(sf::Color::Black);
    setupText(creditBtn, "CREDIT", width/2.0f, 500.0f);
    creditBtn.setOutlineThickness(5.f);
    creditBtn.setOutlineColor(sf::Color::Black);

    
    setupText(creditList, "Member 1: 680610762\nMember 2: 680610782\nMember 3: 680610793\nMember 4: 680610842\n\n", width/2.0f + 85.0f, height/2.0f + 50.0f );
    
    creditList.setCharacterSize(30);
    creditList.setOutlineThickness(5.f);
    creditList.setOutlineColor(sf::Color::Black);

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
            if (event.type == sf::Event::Closed) return 0; 

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (showCredit) {
                    showCredit = false; // ถ้าหน้า Credit เปิดอยู่ คลิกตรงไหนก็ได้เพื่อปิด
                } else {
                    if (playBtn.getGlobalBounds().contains(mousePosF)) return 1; 
                    if (loadBtn.getGlobalBounds().contains(mousePosF)) return 2;
                    if (creditBtn.getGlobalBounds().contains(mousePosF)) showCredit = true; // เปิดหน้า Credit
                    if (exitBtn.getGlobalBounds().contains(mousePosF)) return 0; 
                }
            }
        }

        window.clear();
        if (hasBackground) window.draw(bgSprite);

        if (showCredit) {
            // วาดหน้า Credit
            window.draw(title);
            window.draw(creditList); // วาดรายชื่อสมาชิก
        } else {
            // วาดหน้าเมนูหลักปกติ และจัดการ Effect ปุ่ม Hover
            playBtn.setFillColor(playBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            loadBtn.setFillColor(loadBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            creditBtn.setFillColor(creditBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);
            exitBtn.setFillColor(exitBtn.getGlobalBounds().contains(mousePosF) ? sf::Color::Red : sf::Color::White);

            window.draw(title);
            window.draw(playBtn);
            window.draw(loadBtn);
            window.draw(creditBtn);
            window.draw(exitBtn);
        }
        window.display();
    }
    return 0;
}