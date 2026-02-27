//มุกมิก
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Player.h"
#include "Menu.h"
#include "camera.h"
#include "Playercombat.h"
#include "Enemy.h"
#include "FirstPage.h"
#include "SettingsPage.h" // เพิ่ม 

enum class State {Preparing,Player_turn,Enemy_turn,Check_win,Finished};

int main() {
    // หน้าจอหน้าต่างหลักแค่ครั้งเดียว
    sf::RenderWindow window(sf::VideoMode(800, 800), "test rabob eieieeeieiie");

    FirstPage firstPage(800.f, 800.f);
    SettingsPage settingsPage(800.f, 800.f); // หน้าตั้งค่า
    
    float gameVolume = 75.f; // ตัวแปรเก็บระดับเสียงเริ่มต้น

    // ลูปครอบชั้นนอกสุด เพื่อให้กด Back จาก Setting กลับมาหน้าแรกได้
    while (window.isOpen()) {
        
        int choice = firstPage.run(window); // โค้ดจะหยุดรอหน้านี้จนกว่าจะมีการคลิก

        // ถ้าผู้เล่นกดออก หรือปิดหน้าต่าง ให้จบโปรแกรม
        if (choice == -1 || choice == 0) {
            window.close();
            return 0;
        }
        
        // ถ้าผู้เล่นกด Play (เข้าเกม)
        else if (choice == 1) {
            
            // ==========================================
            // โค้ดเกมเดิมของคุณทั้งหมด อยู่ตรงนี้โดยไม่มีการแก้ลอจิกครับ!
            // ==========================================
            sf::CircleShape shape(100.f);
            shape.setFillColor(sf::Color::Blue);
            shape.setPosition(sf::Vector2f(400.f,300.f));
            
            PlayerC hero("Hero",100,45,40,3,"../eps1.png");
            hero.sprite.setPosition(200,400);
            
            Enemy boss("HEE",150,20,5,0,"../eps1.png");
            boss.sprite.setPosition(600 ,400);

            State curentState = State::Player_turn;

            Camera camera;
            sf::Clock clock;
            
            Player luffy("../animate.png");
            Menu menu(window.getSize().x,window.getSize().y);
            luffy.setScale(5,5);
            sf::Font font;
            
            font.loadFromFile("../CRA.ttf");
            if(!font.loadFromFile("../CRA.ttf")){
                printf("Load font fail");
            }

            sf::Text title("HAI HITLER!!!",font,150) ;
            title.setFillColor(sf::Color::Cyan);
            title.setPosition(300,50);

            sf:: RectangleShape s2({150.f,150.f});
            shape.setFillColor(sf::Color::Green);
            shape.setPosition(sf::Vector2f(250.f,250.f));

            sf::Texture eps;
            eps.loadFromFile("../eps1.png");
            
            sf::Texture movements;
            movements.loadFromFile("../cannotbre.png");

            sf::Sprite nice;
            nice.setTexture(eps);
            nice.setPosition(100.f,100.f);

            sf::Sprite move1;
            move1.setTexture(movements);
            move1.setPosition(600.f,500.f);

            sf::Music music;
            if(!music.openFromFile("../EFN.wav")){
                return -1;
            }
            music.setLoop(true);
            music.setPitch(1.2f);
            
            // แก้ให้ใช้ค่าเสียงจากตัวแปร gameVolume และสั่งให้เพลงเล่น
            music.setVolume(gameVolume); 
            music.play(); 
            
            bool returnToMenu = false; // เอาไว้เช็คตอนกดออกไปเมนู

            while (window.isOpen() && !returnToMenu) {
                sf::Event event;
                float dt =clock.restart().asSeconds();
                while (window.pollEvent(event)) { 
                    if (event.type == sf::Event::Closed)
                        window.close();

                    // กดปุ่ม Esc เพื่อกลับหน้าเมนูไปตั้งค่าเสียง
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        returnToMenu = true;
                        music.stop(); 
                    }

                    if(curentState == State::Player_turn && event.type == sf::Event::KeyPressed){
                        hero.isGuarding = false;
                        if(event.key.code == sf::Keyboard::Space){
                            boss.takeDamage(hero.atk);
                            std::cout << "hero attacks: " << hero.atk << std::endl;
                            std::cout << "///////////////////////////////////////////" << std::endl;
                            curentState = State::Check_win;
                        }else if(event.key.code == sf::Keyboard::F ){
                            hero.isGuarding = true;
                            curentState = State::Enemy_turn;
                        }
                    }
                

                    if(curentState == State::Check_win){
                        if(boss.hp<=0){ 
                            boss.hp = 0 ;
                            curentState = State::Finished;}
                        else curentState = State::Enemy_turn;}
                        
                    if(curentState == State::Enemy_turn){    

                        boss.takeAction(hero);
                        if(hero.hp <= 0 ){
                            hero.hp = 0;
                            curentState = State::Finished;

                        }else{
                            curentState = State::Player_turn;
                        }
                    }
                    if(curentState == State::Check_win){
                        if(boss.hp <= 0){
                            boss.hp = 0;
                        
                            curentState = State::Finished;

                        }else{
                            curentState = State::Enemy_turn;
                        }
                    }
                }
                
                window.clear();
                if(curentState == State::Finished){
                    static bool print = false ;
                    if(!print){
                    if(hero.hp <= 0){
                        std::cout << "you lose" << std::endl;
                    }else if(boss.hp <= 0){
                        std::cout << "yeyeye" << std::endl;
                    }
                    print = true;
                }
                }
                hero.draw(window);
                boss.draw(window);
            
                window.display();
            }
        } // จบเงื่อนไข if (choice == 1)
        
        // ถ้าผู้เล่นกด Settings จากหน้าแรก
        else if (choice == 2) {
            settingsPage.run(window, gameVolume); 
        }
    } 

    return 0;
}