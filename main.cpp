#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include "Combat.h"
#include "Weapon.h"
#include "Player.h"
#include "Monster.h"
#include "Potion.h"

using namespace std;

int main() {
    // 1. Setup พื้นฐาน
    srand(static_cast<unsigned>(time(0)));
    sf::RenderWindow window(sf::VideoMode(100, 100), "RPG Server");

    // 2. Initial Player State
    Player player;
    player.maxHp = 100;
    player.hp = player.maxHp;
    player.wallet.balance = 100; 
    player.level = 1;
    player.exp = 0;
    
    // เรียกใช้ selectWeapon ตามไฟล์ Weapon.h เดิมของคุณ
    player.weapon = WeaponFactory::selectWeapon(1); 

    Potion potion;
    
    cout << "========================================" << endl;
    cout << "      WELCOME TO THE TERMINAL RPG       " << endl;
    cout << "========================================" << endl;

    int monsterCount = 0;

    // 3. Game Loop
    while (player.isAlive() && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        monsterCount++;
        
        Monster monster(monsterCount); 
        monster.name = "Monster #" + to_string(monsterCount);
        monster.maxHp = 50 + (monsterCount * 10);
        monster.hp = monster.maxHp;
        monster.maxDmg = 5 + (monsterCount * 2);

        // สร้าง Summary เพื่อรับค่าสถิติจาก Combat::start
        Summary summary;
        
        cout << "\n>>> Adventure #" << monsterCount << " begins!" << endl;
        cout << "A wild " << monster.name << " (HP: " << monster.hp << ") appeared!" << endl;
        
        // เข้าสู่โหมดต่อสู้
        bool victory = Combat::start(player, monster, potion, summary);

        if (victory) {
            cout << "\n----------------------------------------" << endl;
            cout << "  VICTORY! You defeated " << monster.name << endl;
            
            // --- ส่วนที่เพิ่ม: แสดงสถิติการสู้รอบนี้ ---
            cout << "  Battle Turns: " << summary.turns << " turns | Total Damage: " << summary.totalDmg << " dmg" << endl;
            cout << "----------------------------------------" << endl;
            
            // 4. Rewards & Leveling
            int goldGained = (rand() % 31) + 20; 
            player.wallet.addMoney(goldGained);
            
            int difficultyBonus = (monster.maxHp / 10) + monster.maxDmg;
            int gainedExp = (rand() % 100) + difficultyBonus;
            player.exp += gainedExp;

            cout << "  + " << goldGained << " Gold" << endl;
            cout << "  + " << gainedExp << " EXP" << endl;

            while (player.exp >= player.getNextLevelExp()) {
                player.exp -= player.getNextLevelExp();
                player.level++;
                
                player.maxHp += 20; 
                player.hp = player.maxHp; 
                player.baseMaxDmg += 5;   
                
                cout << "\n  [!] LEVEL UP! Current Level: " << player.level << endl;
                cout << "  [!] Max HP increased to: " << player.maxHp << endl;
                cout << "  [!] Base Attack increased!" << endl;
            }
            cout << "----------------------------------------" << endl;
            
            cout << "Press ENTER to continue journey..." << endl;
            cin.ignore();
            cin.get();
        } else {
            cout << "\n>>>> GAME OVER <<<<" << endl;
            cout << "You were defeated by " << monster.name << endl;
            cout << "Total monsters defeated: " << monsterCount - 1 << endl;
            break;
        }
    }

    // 5. สรุปผลหลังจบเกม
    cout << "\n========== FINAL STATS ==========" << endl;
    cout << "Last Level: " << player.level << endl;
    cout << "Total Gold: " << player.wallet.balance << " G" << endl;
    cout << "=================================" << endl;

    if (player.weapon != nullptr) delete player.weapon;
    
    return 0;
}