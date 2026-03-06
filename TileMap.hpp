#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm> 
#include <memory> 

using json = nlohmann::json;

const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    struct Warp { sf::FloatRect rect; std::string nextMap; };
    std::vector<int> collisionLayer;
    std::vector<Warp> warps;
    int mapW = 0, mapH = 0, tileW = 32, tileH = 32;

private:
    struct LayerData { sf::VertexArray va; std::shared_ptr<sf::Texture> tex; }; 
    std::vector<LayerData> m_layers;

    struct TilesetData {
        int firstGid;
        std::shared_ptr<sf::Texture> tex; 
        int columns;
        int tsTileW;
        int tsTileH;
    };
    std::vector<TilesetData> m_tilesets;

public:
    bool load(const std::string& jsonFile) {
        std::ifstream file(jsonFile);
        if (!file.is_open()) {
            std::cout << "ERROR: Cannot open " << jsonFile << std::endl;
            return false;
        }
        json data; file >> data;

        m_layers.clear(); m_tilesets.clear(); warps.clear(); collisionLayer.clear();
        mapW = data["width"]; mapH = data["height"];
        tileW = data["tilewidth"]; tileH = data["tileheight"];

        // เซ็ตเริ่มต้นให้ "เดินไม่ได้เลย" (Solid) ทั่วทั้งแมพ
        collisionLayer.assign(mapW * mapH, 1);

        for (auto& ts : data["tilesets"]) {
            TilesetData td;
            td.firstGid = ts["firstgid"];
            td.tsTileW = ts.contains("tilewidth") ? (int)ts["tilewidth"] : tileW;
            td.tsTileH = ts.contains("tileheight") ? (int)ts["tileheight"] : tileH;
            
            std::string imgPath = ts["image"];
            size_t lastSlash = imgPath.find_last_of("/\\");
            std::string fileName = (lastSlash == std::string::npos) ? imgPath : imgPath.substr(lastSlash + 1);
            
            td.tex = std::make_shared<sf::Texture>(); 
            if (td.tex->loadFromFile(fileName)) {
                td.columns = ts.contains("columns") ? (int)ts["columns"] : (td.tex->getSize().x / td.tsTileW);
                m_tilesets.push_back(td);
            } else {
                std::cout << "ERROR: Load Texture Failed -> " << fileName << std::endl;
            }
        }

        std::sort(m_tilesets.begin(), m_tilesets.end(), [](const TilesetData& a, const TilesetData& b) {
            return a.firstGid > b.firstGid;
        });

        for (auto& layer : data["layers"]) {
            if (layer["type"] == "tilelayer") {
                auto d = layer["data"].get<std::vector<unsigned int>>(); 
                
                // 🟢 ระบบกรองชื่อเลเยอร์แบบขั้นเทพ (กันบั๊กพิมพ์ผิด/เคาะสเปซบาร์)
                std::string ln = layer["name"];
                std::transform(ln.begin(), ln.end(), ln.begin(), ::tolower); // แปลงเป็นพิมพ์เล็กหมด
                ln.erase(std::remove(ln.begin(), ln.end(), ' '), ln.end());  // ตัดช่องว่างทิ้งหมด

                bool isWalkableLayer = false;
                
                if (jsonFile.find("village") != std::string::npos) { isWalkableLayer = (ln == "path"); }
                else if (jsonFile.find("homie") != std::string::npos) { isWalkableLayer = (ln == "floor" || ln == "prom" || ln == "layer3"); }
                else if (jsonFile.find("tunnel") != std::string::npos) { isWalkableLayer = (ln == "floor"); }
                else if (jsonFile.find("lastboss") != std::string::npos) { isWalkableLayer = (ln == "floor"); }
                else if (jsonFile.find("store") != std::string::npos) { isWalkableLayer = (ln == "prom" || ln == "floor"); }
                else if (jsonFile.find("church") != std::string::npos) { isWalkableLayer = (ln == "prom" || ln == "floor"); }
                else if (jsonFile.find("underground") != std::string::npos) { isWalkableLayer = (ln == "prom" || ln == "floor"); }
                
                // 🟢 เพิ่ม hima ให้ abandon ตรงนี้แล้ว!
                else if (jsonFile.find("abandon") != std::string::npos) { isWalkableLayer = (ln == "path" || ln == "hima"); }

                std::map<int, LayerData> layerVertexArrays;
                for (auto& ts : m_tilesets) {
                    layerVertexArrays[ts.firstGid].tex = ts.tex;
                    layerVertexArrays[ts.firstGid].va.setPrimitiveType(sf::Quads);
                }

                for (int y = 0; y < mapH; y++) {
                    for (int x = 0; x < mapW; x++) {
                        int idx = x + y * mapW;
                        unsigned raw_gid = d[idx];
                        unsigned gid = raw_gid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                        if (gid == 0) continue; // ข้ามช่องว่างไป

                        // อัปเดตการชน
                        if (isWalkableLayer) {
                            collisionLayer[idx] = 0; // เลเยอร์ทางเดิน = เดินเหยียบได้
                        } else {
                            collisionLayer[idx] = 1; // เลเยอร์อื่นๆ = เป็นกำแพงชนหมด!
                        }

                        TilesetData* targetTs = nullptr;
                        for (auto& ts : m_tilesets) {
                            if (gid >= (unsigned)ts.firstGid) {
                                targetTs = &ts;
                                break;
                            }
                        }

                        if (!targetTs) continue;

                        int lid = gid - targetTs->firstGid;
                        int tu = lid % targetTs->columns;
                        int tv = lid / targetTs->columns;

                        sf::Vertex q[4];
                        float px = x * tileW;
                        float py = y * tileH;
                        float tw = targetTs->tsTileW;
                        float th = targetTs->tsTileH;

                        q[0].position = {px, py};
                        q[1].position = {px + tw, py};
                        q[2].position = {px + tw, py + th};
                        q[3].position = {px, py + th};

                        float tx = tu * tw;
                        float ty = tv * th;

                        q[0].texCoords = {tx, ty};
                        q[1].texCoords = {tx + tw, ty};
                        q[2].texCoords = {tx + tw, ty + th};
                        q[3].texCoords = {tx, ty + th};

                        auto& va = layerVertexArrays[targetTs->firstGid].va;
                        for(int i=0;i<4;i++) va.append(q[i]);
                    }
                }

                for (auto& [fgid, ld] : layerVertexArrays) {
                    if (ld.va.getVertexCount() > 0) m_layers.push_back(ld);
                }
            } 
            else if (layer["type"] == "objectgroup") {
                for (auto& obj : layer["objects"]) {
                    Warp wp; 
                    wp.rect = { (float)obj["x"], (float)obj["y"], (float)obj["width"], (float)obj["height"] };
                    
                    if (obj.contains("properties")) {
                        for (auto& p : obj["properties"]) {
                            std::string pName = p["name"];
                            std::string lowerName = pName;
                            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                            
                            if (lowerName == "nextmap" || lowerName == "naxtmap") {
                                wp.nextMap = p["value"].get<std::string>(); 
                            }
                        }
                    }
                    warps.push_back(wp);
                }
            }
        }
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
        for (auto& l : m_layers) { 
            if (l.tex) s.texture = l.tex.get();
            t.draw(l.va, s); 
        }
    }
};
#endif