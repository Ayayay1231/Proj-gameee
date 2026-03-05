// MapItem.h
#ifndef MAPITEM_H
#define MAPITEM_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Player.h" 

using namespace std;

// โครงสร้างที่เพิ่ม imgLoaded และ fallbackBox แล้ว!
struct MapItemObj {
    Item itemData;    
    string mapName;   
    sf::Sprite sprite;
    sf::RectangleShape fallbackBox; 
    bool imgLoaded = false; 
};

class MapItemManager {
public:
    vector<MapItemObj> list;
    map<string, sf::Texture> textureCache;

    void spawnItem(string mapNames, Item data, string imgPath, float x, float y, float scaleX = 1.f, float scaleY = 1.f) {
        MapItemObj newItem;
        newItem.itemData = data;
        newItem.mapName = mapNames;

        if (textureCache.find(imgPath) == textureCache.end()) {
            if (!textureCache[imgPath].loadFromFile(imgPath)) {
                cout << "[ERROR] Cannot load item image -> " << imgPath << endl;
            }
        }

        if (textureCache[imgPath].getSize().x > 0) {
            newItem.sprite.setTexture(textureCache[imgPath]);
            newItem.sprite.setPosition(x, y);
            newItem.sprite.setScale(scaleX, scaleY);
            newItem.imgLoaded = true;
        } else {
            // ถ้าโหลดรูปไม่ติด ให้สร้างกล่องสีชมพูขนาด 40x40 แทน
            newItem.fallbackBox.setSize({40.f, 40.f});
            newItem.fallbackBox.setFillColor(sf::Color::Magenta);
            newItem.fallbackBox.setPosition(x, y);
            newItem.imgLoaded = false;
        }
        
        list.push_back(newItem);
    }

    void drawAll(sf::RenderWindow& window, string currentMap) {
        for(auto& item : list){
            if (item.mapName == currentMap) {
                if (item.imgLoaded) window.draw(item.sprite);
                else window.draw(item.fallbackBox);
            }
        }
    }
};
#endif