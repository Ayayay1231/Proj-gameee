// main.cpp
#include "Game.hpp"
#include <ctime>
#include <cstdlib>

int main() {
    // 1. ตั้งค่า seed สุ่มตัวเลขแค่ครั้งเดียว
    srand(static_cast<unsigned>(time(0)));
    
    // 2. สร้างและรันเกม (มันจะไปเปิดหน้าต่างใน Game.hpp แทน)
    Game game;
    game.run();
    
    return 0;
}

