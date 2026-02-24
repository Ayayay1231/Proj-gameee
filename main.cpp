// main.cpp
#include "Game.hpp"
#include <ctime>
#include <cstdlib>

int main() {
    // ตั้งค่า seed สุ่มครั้งเดียว
    srand(static_cast<unsigned>(time(0)));
    
    // สร้างและรันเกม (หน้าต่างอยู่ที่ Game.hpp)
    Game game;
    game.run();
    
    return 0;
}
