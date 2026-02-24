#ifndef NPC_H
#define NPC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

// 1. คลาส NPC 
class NPC {
public:
    
    string name,mapNames;
    vector<string> messages;
    sf::Sprite sprite;

    // --- เพิ่ม float scaleX, float scaleY เข้ามาตรงนี้ ---
    NPC(string mName , string npcName, vector<string> npcMsgs, float startX, float startY, float scaleX = 1.f, float scaleY = 1.f) {
        name = npcName;
        mapNames = mName;
        messages = npcMsgs;
        sprite.setPosition(startX, startY);
        
        // เอาค่าสเกลที่รับมาไปตั้งค่าขนาด
        sprite.setScale(scaleX, scaleY); 
    }

    bool isPlayerNear(sf::FloatRect playerBounds) {
        sf::FloatRect interactArea = playerBounds;
        interactArea.left -= 30; interactArea.top -= 30;
        interactArea.width += 60; interactArea.height += 60;
        return interactArea.intersects(sprite.getGlobalBounds());
    }
};

// 2. คลาส NPCManager
class NPCManager {
public:
    vector<NPC> list; 
    map<string, sf::Texture> textureCache;

    // --- เพิ่ม float scaleX, float scaleY เข้ามาตรงนี้ด้วย ---
    void spawnNPC(string mapNames ,string name, vector<string> msgs, string imgPath, float x, float y, float scaleX = 1.f, float scaleY = 1.f) {
        
        if (textureCache.find(imgPath) == textureCache.end()) {
            sf::Texture tex;
            if (tex.loadFromFile(imgPath)) {
                textureCache[imgPath] = tex;
            } else {
                cout << "Error: Cannot load " << imgPath << endl;
            }
        }

        // โยนค่าสเกลส่งต่อไปให้ตัว NPC
        NPC newNpc(mapNames,name, msgs, x, y, scaleX, scaleY);
        list.push_back(newNpc);
        list.back().sprite.setTexture(textureCache[imgPath]);
    }

    void drawAll(sf::RenderWindow& window, string currentMap) {
        for(auto& npc : list){
        if (npc.mapNames == currentMap) {
                window.draw(npc.sprite);
            }
    }
}
};
#endif