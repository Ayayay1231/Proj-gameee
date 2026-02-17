#ifndef TILEMAP_HPP
#define TILEMAP_HPP
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm> // สำหรับแปลงตัวพิมพ์เล็ก

using json = nlohmann::json;

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    struct Warp { sf::FloatRect rect; std::string nextMap; };
    std::vector<int> collisionLayer;
    std::vector<Warp> warps;
    int mapW = 0, mapH = 0, tileW = 32, tileH = 32;

private:
    struct LayerData { sf::VertexArray va; sf::Texture* tex; };
    std::vector<LayerData> m_layers;
    std::map<int, sf::Texture> m_textures;

public:
    bool load(const std::string& jsonFile) {
        std::ifstream file(jsonFile);
        if (!file.is_open()) {
            std::cout << "ERROR: Cannot open " << jsonFile << std::endl;
            return false;
        }
        json data; file >> data;

        m_layers.clear(); m_textures.clear(); warps.clear(); collisionLayer.clear();
        mapW = data["width"]; mapH = data["height"];
        tileW = data["tilewidth"]; tileH = data["tileheight"];

        for (auto& ts : data["tilesets"]) {
            sf::Texture tex;
            std::string imgPath = ts["image"];
            size_t lastSlash = imgPath.find_last_of("/\\");
            std::string fileName = (lastSlash == std::string::npos) ? imgPath : imgPath.substr(lastSlash + 1);
            if (tex.loadFromFile(fileName)) m_textures[ts["firstgid"]] = tex;
        }

        for (auto& layer : data["layers"]) {
            if (layer["type"] == "tilelayer") {
                auto d = layer["data"].get<std::vector<int>>();
                if (layer["name"] == "walls" || layer["name"] == "House") collisionLayer = d;

                for (auto& [firstGid, texture] : m_textures) {
                    LayerData ld; ld.tex = &m_textures[firstGid]; ld.va.setPrimitiveType(sf::Quads);
                    for (int y = 0; y < mapH; y++) {
                        for (int x = 0; x < mapW; x++) {
                            int gid = d[x + y * mapW];
                            auto it = m_textures.upper_bound(gid);
                            if (it != m_textures.begin()) --it;
                            if (it->first != firstGid || gid == 0) continue;
                            int lid = gid - firstGid;
                            int tu = lid % (ld.tex->getSize().x / tileW);
                            int tv = lid / (ld.tex->getSize().x / tileW);
                            sf::Vertex q[4];
                            q[0].position = {(float)x*tileW, (float)y*tileH};
                            q[1].position = {(float)(x+1)*tileW, (float)y*tileH};
                            q[2].position = {(float)(x+1)*tileW, (float)(y+1)*tileH};
                            q[3].position = {(float)x*tileW, (float)(y+1)*tileH};
                            q[0].texCoords = {(float)tu*tileW, (float)tv*tileH};
                            q[1].texCoords = {(float)(tu+1)*tileW, (float)tv*tileH};
                            q[2].texCoords = {(float)(tu+1)*tileW, (float)(tv+1)*tileH};
                            q[3].texCoords = {(float)tu*tileW, (float)(tv+1)*tileH};
                            for(int i=0; i<4; i++) ld.va.append(q[i]);
                        }
                    }
                    if (ld.va.getVertexCount() > 0) m_layers.push_back(ld);
                }
            } 
            else if (layer["type"] == "objectgroup") {
                std::cout << "Checking Object Layer: " << layer["name"] << std::endl;
                for (auto& obj : layer["objects"]) {
                    Warp wp; 
                    wp.rect = { (float)obj["x"], (float)obj["y"], (float)obj["width"], (float)obj["height"] };
                    
                    if (obj.contains("properties")) {
                        for (auto& p : obj["properties"]) {
                            std::string pName = p["name"];
                            // แปลงชื่อ property เป็นตัวพิมพ์เล็กเพื่อเช็ค
                            std::string lowerName = pName;
                            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                            
                            if (lowerName == "nextmap" || lowerName == "naxtmap") {
                                wp.nextMap = p["value"];
                                std::cout << "FOUND WARP PROPERTY: " << pName << " = " << wp.nextMap << std::endl;
                            }
                        }
                    }
                    warps.push_back(wp);
                }
            }
        }
        std::cout << "Total Warps Loaded: " << warps.size() << " from file: " << jsonFile << std::endl;
        return true;
    }

    bool isSolid(float x, float y) const {
        if (collisionLayer.empty()) return false;
        int tx = (int)x / tileW; int ty = (int)y / tileH;
        if (tx < 0 || tx >= mapW || ty < 0 || ty >= mapH) return true;
        return collisionLayer[ty * mapW + tx] != 0;
    }

    void draw(sf::RenderTarget& t, sf::RenderStates s) const override {
        s.transform *= getTransform();
        for (auto& l : m_layers) { s.texture = l.tex; t.draw(l.va, s); }
    }
};
#endif