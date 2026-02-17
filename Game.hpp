#ifndef GAME_HPP
#define GAME_HPP
#include "TileMap.hpp"
#include <iostream>

class Game {
private:
    sf::RenderWindow window;
    TileMap map;
    
    sf::Sprite player;
    sf::Texture playerTexture; 
    
    sf::RectangleShape fadeRect;
    float alpha = 0;          
    bool isFading = false;    
    std::string pendingMap = ""; 

public:
    Game() {
        window.create(sf::VideoMode(1280, 720), "RPG Game - Position Fix");
        window.setFramerateLimit(60);
        
        map.load("map_world.json");
        
        if (!playerTexture.loadFromFile("player2.png")) {
            std::cout << "Error: Could not load player2.png" << std::endl;
        }
        player.setTexture(playerTexture);
        sf::FloatRect bounds = player.getLocalBounds();
        player.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        
        player.setPosition(600.f, 600.f); 

        fadeRect.setSize({1280, 720}); 
        fadeRect.setFillColor(sf::Color(0, 0, 0, 0)); 
    }

    void update() {
        if (isFading) {
            alpha += 8; 
            if (alpha >= 255) {
                alpha = 255;
                if (map.load(pendingMap)) {
                    std::cout << "Warped to: " << pendingMap << std::endl;
                    player.setPosition(300.f, 300.f); 
                    isFading = false; 
                }
            }
        } else {
            if (alpha > 0) {
                alpha -= 8; 
                if (alpha < 0) alpha = 0;
            }

            if (alpha < 100) { 
                sf::Vector2f move(0, 0);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= 4;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 4;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move.x -= 4;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 4;

                sf::FloatRect p = player.getGlobalBounds();
                p.left += 5; p.top += 5; p.width -= 10; p.height -= 10;

                if (!map.isSolid(p.left + move.x, p.top) && !map.isSolid(p.left + p.width + move.x, p.top + p.height))
                    player.move(move.x, 0);
                if (!map.isSolid(p.left, p.top + move.y) && !map.isSolid(p.left + p.width, p.top + p.height + move.y))
                    player.move(0, move.y);
            }

            // ระบบวาร์ป (กด E)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                sf::FloatRect warpCheck = player.getGlobalBounds();
                warpCheck.left -= 25; warpCheck.top -= 25; warpCheck.width += 50; warpCheck.height += 50;

                for (auto& w : map.warps) {
                    if (warpCheck.intersects(w.rect)) {
                        if (!w.nextMap.empty() && !isFading && alpha == 0) {
                            isFading = true;       
                            pendingMap = w.nextMap;
                            break;
                        }
                    }
                }
            }
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    }

    void render() {
        window.clear(); 
        window.draw(map); 
        window.draw(player); 
        window.draw(fadeRect); 
        window.display();
    }

    void run() {
        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();
            }
            update(); 
            render();
        }
    }
};
#endif