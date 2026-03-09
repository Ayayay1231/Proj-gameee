#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Cutscene {
public:
    vector<string> imagePaths;
    vector<string> texts;
    int currentIndex = 0;
    
    sf::Texture currentTexture;
    sf::Sprite currentSprite;

    string fullMsg = "";
    string currentMsg = "";
    int charIdx = 0;
    sf::Clock typeClock;

    bool isPlaying = false;

    // เรียกใช้ฟังก์ชันนี้เมื่อต้องการเริ่มคัตซีน
    void start(vector<string> imgs, vector<string> txts) {
        imagePaths = imgs;
        texts = txts;
        currentIndex = 0;
        isPlaying = true;

        if (!imagePaths.empty()) {
            loadCurrentSlide();
        }
    }

    void loadCurrentSlide() {
        if (currentTexture.loadFromFile(imagePaths[currentIndex])) {
            currentSprite.setTexture(currentTexture);
            
            // ขยายรูปให้เต็มจอ 1280x720 พอดี
            sf::FloatRect bounds = currentSprite.getLocalBounds();
            currentSprite.setScale(1280.f / bounds.width, 720.f / bounds.height);
            currentSprite.setPosition(0.f, 0.f);
        } else {
            cout << "Cutscene Error: Cannot load " << imagePaths[currentIndex] << endl;
        }
        
        fullMsg = texts[currentIndex];
        currentMsg = "";
        charIdx = 0;
        typeClock.restart();
    }

    // จัดการการกด Spacebar / Enter
    void handleInput(sf::Event& e) {
        if (!isPlaying) return;

        if (e.type == sf::Event::KeyPressed && (e.key.code == sf::Keyboard::Space || e.key.code == sf::Keyboard::Enter)) {
            if (charIdx < fullMsg.length()) {
                // กดข้ามแอนิเมชันตัวหนังสือ
                currentMsg = fullMsg;
                charIdx = fullMsg.length();
            } else {
                // ไปหน้าถัดไป
                currentIndex++;
                if (currentIndex < imagePaths.size()) {
                    loadCurrentSlide();
                } else {
                    isPlaying = false; // จบคัตซีน
                }
            }
        }
    }

    // แอนิเมชันพิมพ์ดีด
    void updateText() {
        if (!isPlaying) return;
        if (charIdx < fullMsg.length() && typeClock.getElapsedTime().asMilliseconds() > 25) {
            currentMsg += fullMsg[charIdx];
            charIdx++;
            typeClock.restart();
        }
    }

    // วาดคัตซีนลงจอ (ยืมกล่องข้อความจาก Game.hpp มาใช้)
    void render(sf::RenderWindow& window, sf::RectangleShape& dBox, sf::Text& dName, sf::Text& dText) {
        if (!isPlaying) return;

        window.clear(sf::Color::Black);
        window.draw(currentSprite);

        window.draw(dBox);
        
        dName.setString("STORY"); // ชื่อผู้พูด (ตั้งเป็น STORY หรือเปลี่ยนได้ตามใจชอบ)
        window.draw(dName);

        dText.setString(currentMsg);
        window.draw(dText);
    }
};

#endif