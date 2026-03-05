#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm> 

using json = nlohmann::json;

// Bitmask ป้องกันแมพแตกเวลาเผลอหมุนกระเบื้องใน Tiled
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
    struct LayerData { sf::VertexArray va; sf::Texture* tex; };
    std::vector<LayerData> m_layers;

    // เก็บข้อมูล Tileset ให้แน่นขึ้น ป้องกัน Texture หลุด
    struct TilesetData {
        int firstGid;
        sf::Texture tex;
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

        // 1. โหลดข้อมูล Tilesets ให้ครบก่อน
        for (auto& ts : data["tilesets"]) {
            TilesetData td;
            td.firstGid = ts["firstgid"];
            td.tsTileW = ts.contains("tilewidth") ? (int)ts["tilewidth"] : tileW;
            td.tsTileH = ts.contains("tileheight") ? (int)ts["tileheight"] : tileH;
            
            std::string imgPath = ts["image"];
            size_t lastSlash = imgPath.find_last_of("/\\");
            std::string fileName = (lastSlash == std::string::npos) ? imgPath : imgPath.substr(lastSlash + 1);
            
            if (td.tex.loadFromFile(fileName)) {
                // คำนวณคอลัมน์ของแผ่นนั้นๆ (กันภาพสัดส่วนเพี้ยน)
                td.columns = ts.contains("columns") ? (int)ts["columns"] : (td.tex.getSize().x / td.tsTileW);
                m_tilesets.push_back(std::move(td));
            } else {
                std::cout << "ERROR: Load Texture Failed -> " << fileName << std::endl;
            }
        }

        // เรียง Tileset จาก ID มากไปน้อย เพื่อให้ค้นหาแผ่นที่ถูกต้องง่ายขึ้น
        std::sort(m_tilesets.begin(), m_tilesets.end(), [](const TilesetData& a, const TilesetData& b) {
            return a.firstGid > b.firstGid;
        });

        // 2. ลุยโหลด Layers ของจริง
        for (auto& layer : data["layers"]) {
            if (layer["type"] == "tilelayer") {
                // โหลดเป็น unsigned เพื่อรองรับ Bitmask ตัวเลขใหญ่ๆ
                auto d = layer["data"].get<std::vector<unsigned int>>(); 
                
                if (layer["name"] == "walls" || layer["name"] == "House") {
                    for (auto val : d) {
                        // ตัด Bitmask ทิ้งก่อนเก็บลง Collision
                        collisionLayer.push_back((int)(val & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG)));
                    }
                }

                // สร้างกระดาษวาดรูป (VertexArray) แยกตามแผ่น Tileset ในเลเยอร์นี้
                std::map<int, LayerData> layerVertexArrays;
                for (auto& ts : m_tilesets) {
                    layerVertexArrays[ts.firstGid].tex = &ts.tex;
                    layerVertexArrays[ts.firstGid].va.setPrimitiveType(sf::Quads);
                }

                for (int y = 0; y < mapH; y++) {
                    for (int x = 0; x < mapW; x++) {
                        unsigned raw_gid = d[x + y * mapW];
                        
                        // 🔥 พระเอกอยู่ตรงนี้: สาง Bitmask กลับหน้า/หลัง/บน/ล่าง ทิ้งให้หมด!
                        unsigned gid = raw_gid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                        if (gid == 0) continue; // ข้ามช่องว่าง

                        // หาว่า ID นี้เป็นของ Texture แผ่นไหน
                        TilesetData* targetTs = nullptr;
                        for (auto& ts : m_tilesets) {
                            if (gid >= ts.firstGid) {
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

                        // ยัดสี่เหลี่ยมลง VertexArray ให้ถูกแผ่น
                        auto& va = layerVertexArrays[targetTs->firstGid].va;
                        for(int i=0; i; i<4; i++) va.append(q[i]);
                    }
                }

                // เก็บเฉพาะแผ่นที่ถูกวาดจริงๆ ลงในคอลเล็กชั่นหลัก
                for (auto& [fgid, ld] : layerVertexArrays) {
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
        for (auto& l : m_layers) { 
            s.texture = l.tex; 
            t.draw(l.va, s); 
        }
    }
};
#endif
