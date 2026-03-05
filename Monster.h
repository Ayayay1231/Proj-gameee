#ifndef MONSTER_H
#define MONSTER_H
#include <SFML/Graphics.hpp>
#include <string>
#include <cstdlib>

using namespace std;

class Monster {
public:
    string name;
    int hp, maxHp, maxDmg;

    // สถานะเดิม
    int bleedTurn = 0, stunTurn = 0, vulnTurn = 0;

    // สถานะโลกซอมบี้
    int  infectedTurn = 0;
    int  burnTurn     = 0;   // 🔥
    bool rotten       = false;
    bool frenzy       = false;

    sf::Texture texture;
    sf::Sprite  sprite;

    Monster(int rank) {
        name   = "Zombie #" + to_string(rank);
        maxHp  = 50 + (rank * 10);
        hp     = maxHp;
        maxDmg = 5  + (rank * 2);
        if (texture.loadFromFile("monster1.png")) {
            sprite.setTexture(texture);
            sprite.setPosition(150.f, 50.f);
        }
    }

    // ✅ สุ่มติด infected 30% — เรียกหลังโจมตีธรรมดา [C]
    void tryInflictInfected() {
        if (infectedTurn > 0) return;
        if (rand() % 100 < 30)
            infectedTurn = 3;
    }

    // ✅ สุ่มติด burn 35% — เรียกหลังยิงธนูไฟ [F]
    void tryInflictBurn() {
        if (burnTurn > 0) return;
        if (rand() % 100 < 35)
            burnTurn = 3;
    }

    // ✅ ประมวลผลสถานะต้นเทิร์น — คืน log ให้ Combat แสดง
    string applyStatusEffects() {
        string log = "";

        // 🦠 infected: ลด 3 HP/เทิร์น
        if (infectedTurn > 0) {
            hp -= 3;
            log += name + " takes 3 dmg from INFECTION! \n";
            infectedTurn--;
            if (infectedTurn == 0) {
                rotten = true;
                log += name + " has ROTTED!\n";
            }
        }

        // 🔥 burn: ลด 5 HP/เทิร์น (x2 ถ้า rotten)
        if (burnTurn > 0) {
            int burnDmg = rotten ? 10 : 5;
            hp -= burnDmg;
            log += name + " takes " + to_string(burnDmg) + " dmg from BURN!"
                 + (rotten ? " (x2 ROTTEN!)" : "") + "\n";
            burnTurn--;
        }

        // 👁️ frenzy auto-trigger เมื่อ HP < 30%
        if (!frenzy && hp < maxHp * 0.3f) {
            frenzy = true;
            log += name + " enters FRENZY!\n";
        }

        return log;
    }

    // ✅ ดาเมจจริงที่มอนตี (รวม rotten/frenzy)
    int getActualDmg() {
        int dmg = maxDmg;
        if (rotten) dmg = dmg / 2;
        if (frenzy) dmg = dmg * 3 / 2;
        return max(1, dmg);
    }

    // ✅ frenzy ตี 2 ครั้ง/เทิร์น
    int getAttackCount() {
        return frenzy ? 2 : 1;
    }

    // ✅ ตัวคูณดาเมจที่มอนรับ
    float getDefMultiplier() {
        float m = 1.0f;
        if (rotten) m *= 1.3f;
        if (frenzy) m *= 1.2f;
        return m;
    }

    // ✅ infected ป้องกัน stun และ bleed
    bool isImmuneToStun()  const { return infectedTurn > 0; }
    bool isImmuneToBleed() const { return infectedTurn > 0; }

    // ✅ rotten: 40% spawn mini zombie เมื่อตาย
    bool shouldSpawnOnDeath() const {
        return rotten && (rand() % 100 < 40);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};
#endif