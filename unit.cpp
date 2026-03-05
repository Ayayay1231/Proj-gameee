#include "Unit.h"
#include <iostream>
#include <algorithm>

Unit::Unit(std::string n , int h , int m, int atk,int i,std::string image)
   : name(n), hp(h) ,maxHp(h), mp(m),atk(atk),items(i),  isGuarding(false){
    if(!texture.loadFromFile("sprite/eps1.png")){
        std::cout << "load texture failed!" <<std::endl;
    }
    sprite.setTexture(texture);
   };

   void Unit::takeDamage(int damage) //รับดาเมจ
   {
    if (isGuarding)
    {
        damage = damage/4;
        std::cout << "damage has been reduced : " << damage << std::endl;

    }
    hp -= damage ;
    if(hp <= 0){
        hp = 0 ;
        
    }
    std::cout << "Hp "<< name << " left : " << hp << std::endl;
    isGuarding = false;
    /*if(hp <= 0)
    hp = 0;
    //std::cout << "took " << "damage : " << damage << std::endl ;
    isGuarding = false; */
    
   }

   void Unit::heal(int amount){
    if(items > 0){
        hp = std::min(maxHp,hp+amount);
        items--;
        std::cout << "heal" ;


    }else{
        std:: cout << "mod laew jar";
    }
   }

   bool Unit::useSkill(Unit& target){
    if(mp >= 15){
        std::cout << "skill used";
        target.takeDamage(atk*2);
        mp -= 15;
        return true;
    }else{
        std::cout << "not enough mp";
        return false;
    }

   }

   void Unit::showStats() const{
    std::cout << hp << " " << maxHp << " " << mp << " " << hp << std::endl;
   }

   void Unit::draw(sf::RenderWindow& window){
    window.draw(this->sprite);
   }