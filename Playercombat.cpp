#include"Playercombat.h"
#include<iostream>
#include<SFML/Graphics.hpp>

PlayerC::PlayerC(std::string n, int h, int m, int a, int i,std::string image)
: Unit(n,h,m,a,i,image),level(1),exp(0){

};

void PlayerC::usePotion(){
    
    if(items > 0){
        heal(40);
        std::cout << "heal" ;


    }
    
   

}

void PlayerC::gainExp(int amount){
    exp += amount;
    if (exp>= 100)
    {
        level++;
        atk++;
        maxHp+=5;
        hp = maxHp;
        exp = 0;
        std::cout << "lvl up"<<std::endl;
    }
    
}