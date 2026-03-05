#ifndef TILEMAP_HPP
#define TILEMAP_HPP
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

using json = nlohmann::json;

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    struct Warp { sf::FloatRect rect; std::string nextMap; };
    std::vector<Warp> warps;
    std::vector<bool> walkableMap; 
    int mapW = 0, mapH = 0, tileW = 16, tileH = 16; 

private:
    struct LayerData { sf::VertexArray va; sf::Texture* tex; };
    std::vector<LayerData> m_layers;
    std::map<int, sf::Texture> m_textures;

    std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

public:
    bool load(const std::string& jsonFile) {
        std::ifstream file(jsonFile);
        if (!file.is_open()) {
            std::cout << "Error: File not found " << jsonFile << std::endl;
            return false;
        }
        json data; file >> data;

        m_layers.clear(); m_textures.clear(); warps.clear(); 
        mapW = data["width"]; mapH = data["height"];
        tileW = data["tilewidth"]; 
        tileH = data["tileheight"];
        
        walkableMap.assign(mapW * mapH, false);

        // โหลด Tilesets (สำคัญ: ต้องมีไฟล์รูปอยู่ในโฟลเดอร์เดียวกับ .exe)
        for (auto& ts : data["tilesets"]) {
            int firstGid = ts["firstgid"];
            std::string imgPath = ts["image"];
            size_t lastSlash = imgPath.find_last_of("/\\");
            std::string fileName = (lastSlash == std::string::npos) ? imgPath : imgPath.substr(lastSlash + 1);
            
            sf::Texture tex;
            if (tex.loadFromFile(fileName)) {
                m_textures[firstGid] = tex;
            } else {
                std::cout << "Warning: Cannot load texture " << fileName << std::endl;
            }
        }

        // โหลดทุกเลเยอร์แบบไม่กรองชื่อ
        for (auto& layer : data["layers"]) {
            if (layer["type"] == "tilelayer") {
                std::string layerName = toLower(layer["name"]);
                std::vector<int> lData = layer["data"].get<std::vector<int>>();
                
                // ตรวจว่าเลเยอร์นี้คือพื้นหรือไม่ (ถ้าชื่อมีคำเหล่านี้จะเดินได้)
                bool canWalk = (layerName.find("path") != std::string::npos || 
                                layerName.find("floor") != std::string::npos || 
                                layerName.find("ground") != std::string::npos ||
                                layerName.find("prom") != std::string::npos ||
                                layerName.find("grass") != std::string::npos); // เพิ่ม grass ให้ด้วย

                LayerData ld;
                ld.va.setPrimitiveType(sf::Quads);
                ld.va.resize(mapW * mapH * 4);
                ld.tex = nullptr;

                for (int y = 0; y < mapH; y++) {
                    for (int x = 0; x < mapW; x++) {
                        int gid = lData[y * mapW + x];
                        if (gid == 0) continue;

                        // ถ้าเป็นเลเยอร์ที่กำหนดให้เดินได้ ให้บันทึกไว้
                        if (canWalk) walkableMap[y * mapW + x] = true;

                        // วาดทุกอย่าง!
                        auto it = m_textures.upper_bound(gid);
                        if (it != m_textures.begin()) {
                            --it;
                            int firstGid = it->first;
                            sf::Texture& tex = it->second;
                            ld.tex = &tex;

                            int localId = gid - firstGid;
                            int tu = localId % (tex.getSize().x / tileW);
                            int tv = localId / (tex.getSize().x / tileW);

                            sf::Vertex* q = &ld.va[(x + y * mapW) * 4];
                            q[0].position = sf::Vector2f(x * tileW, y * tileH);
                            q[1].position = sf::Vector2f((x + 1) * tileW, y * tileH);
                            q[2].position = sf::Vector2f((x + 1) * tileW, (y + 1) * tileH);
                            q[3].position = sf::Vector2f(x * tileW, (y + 1) * tileH);

                            q[0].texCoords = sf::Vector2f(tu * tileW, tv * tileH);
                            q[1].texCoords = sf::Vector2f((tu + 1) * tileW, tv * tileH);
                            q[2].texCoords = sf::Vector2f((tu + 1) * tileW, (tv + 1) * tileH);
                            q[3].texCoords = sf::Vector2f(tu * tileW, (tv + 1) * tileH);
                        }
                    }
                }
                m_layers.push_back(ld);
            }
            else if (layer["type"] == "objectgroup") {
                for (auto& obj : layer["objects"]) {
                    Warp wp;
                    wp.rect = sf::FloatRect(obj["x"].get<float>(), obj["y"].get<float>(), obj["width"].get<float>(), obj["height"].get<float>());
                    if (obj.contains("properties")) {
                        for (auto& p : obj["properties"]) {
                            if (toLower(p["name"]) == "nextmap") wp.nextMap = p["value"];
                        }
                    }
                    warps.push_back(wp);
                }
            }
        }
        return true;
    }

    bool isSolid(float x, float y) const {
        int tx = (int)x / tileW;
        int ty = (int)y / tileH;
        if (tx < 0 || tx >= mapW || ty < 0 || ty >= mapH) return true;
        // เดินได้เฉพาะที่มาร์คไว้ (ถ้า walkableMap[i] เป็น false คือเดินไม่ได้)
        return !walkableMap[ty * mapW + tx]; 
    }

    void draw(sf::RenderTarget& t, sf::RenderStates s) const override {
        s.transform *= getTransform();
        for (auto& ld : m_layers) {
            if (ld.tex != nullptr) t.draw(ld.va, ld.tex);
        }
    }
};
#endif