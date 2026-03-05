#ifndef NPC_H
#define NPC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class NPC {
public:
    int npcId = 0; // <--- เพิ่ม ID ประจำตัวให้ NPC
    string name, mapNames;
    vector<string> messages;
    sf::Sprite sprite;

    bool hasChoice = false;
    string choiceA, choiceB; 
    string replyA, replyB;   

    bool isEnemy = false;
    int monsterId = 0;

    // แก้ไข Constructor ให้รับค่ายัด ID เข้ามาด้วย
    NPC(int id, string mName , string npcName, vector<string> npcMsgs, float startX, float startY, float scaleX = 1.f, float scaleY = 1.f) {
        npcId = id;
        mapNames = mName;
        name = npcName;
        messages = npcMsgs;
        sprite.setPosition(startX, startY);
        sprite.setScale(scaleX, scaleY); 
    }

    void setChoices(string cA, string cB, string rA, string rB) {
        hasChoice = true;
        choiceA = cA; choiceB = cB;
        replyA = rA; replyB = rB;
    }

    void setAsEnemy(int mId) {
        isEnemy = true;
        monsterId = mId;
    }

    bool isPlayerNear(sf::FloatRect playerBounds) {
        sf::FloatRect interactArea = playerBounds;
        interactArea.left -= 30; interactArea.top -= 30;
        interactArea.width += 60; interactArea.height += 60;
        return interactArea.intersects(sprite.getGlobalBounds());
    }
};

class NPCManager {
public:
    vector<NPC> list; 
    map<string, sf::Texture> textureCache;

    // แก้ไข spawnNPC ให้ต้องใส่เลข ID นำหน้าเสมอ
    void spawnNPC(int id, string mapNames ,string name, vector<string> msgs, string imgPath, float x, float y, float scaleX = 1.f, float scaleY = 1.f) {
        if (textureCache.find(imgPath) == textureCache.end()) {
            sf::Texture tex;
            if (tex.loadFromFile(imgPath)) textureCache[imgPath] = tex;
        }
        NPC newNpc(id, mapNames,name, msgs, x, y, scaleX, scaleY);
        list.push_back(newNpc);
        list.back().sprite.setTexture(textureCache[imgPath]);
    }

    void setLastNPCChoice(string cA, string cB, string rA, string rB) {
        if (!list.empty()) list.back().setChoices(cA, cB, rA, rB);
    }

    void spawnEnemy(string mapNames, int mId, string imgPath, float x, float y, float scaleX = 1.f, float scaleY = 1.f) {
        vector<string> emptyMsg; 
        spawnNPC(0, mapNames, "Enemy", emptyMsg, imgPath, x, y, scaleX, scaleY); // ศัตรูใช้ ID 0 ไปเลย
        list.back().setAsEnemy(mId); 
    }

    void drawAll(sf::RenderWindow& window, string currentMap) {
        for(auto& npc : list){
            if (npc.mapNames == currentMap) window.draw(npc.sprite);
        }
    }
};
#endif