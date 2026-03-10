#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib> 
#include <string>

#include "relationship.h" 
#include "animationHandler.h" 
#include "emotions.h"

using namespace std;

//for tribe animation
enum TribeAnimID {
    UP = 0,    
    DOWN = 1,  
    LEFT = 2,  
    RIGHT = 3  
};

class TribeEntity {
private:
   
    Relationship relation; 
    sf::Vector2f position;
    Emotion currentEmotion;
    
    // callouts
    float timeSinceLastInteraction; 
    float timeNearNeutral;          
    bool isHostile;         
    float assistChance;     
    float callHelpChance;   

    // stats
    float maxAttack;        
    float critChance;       
    int maxHealth;      
    int currentHealth;  
    bool isDead;        
    bool wantCombat;

    // animations
    sf::Texture texture;
    sf::Sprite sprite;
    AnimationHandler* animator; 

    // randomize walking
    sf::Vector2f randomMoveDir;
    float randomMoveTimer;
    float randomMoveMaxTime;

   
    float getDistance(sf::Vector2f p1, sf::Vector2f p2) {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        return sqrt(dx*dx + dy*dy);
    }

    int calculateAttackDamage() {
        float minDmgMult = 0.5f;
        float maxDmgMult = 1.0f;
        float randMult = minDmgMult + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxDmgMult - minDmgMult)));
        float baseDmg = maxAttack * randMult;

        float critRand = static_cast<float>(rand()) / RAND_MAX;
        if (critRand < critChance) {
            cout << "*** CRITICAL HIT! ***" << endl;
            return static_cast<int>(baseDmg * 1.10f);
        }
        return static_cast<int>(baseDmg);
    }

    // behaviors
    
    // allies
    void performAllyBehavior(float deltaTime, sf::Vector2f playerPos) {
        float dist = getDistance(position, playerPos);
        if (dist > 80.0f) {
            sf::Vector2f direction = playerPos - position;
            float len = sqrt(direction.x*direction.x + direction.y*direction.y);
            if (len != 0) direction /= len;
            
            position += direction * 60.0f * deltaTime;
            
            if (abs(direction.x) > abs(direction.y)) {
                if (direction.x > 0) animator->play(RIGHT);
                else animator->play(LEFT);
            } else {
                if (direction.y > 0) animator->play(DOWN);
                else animator->play(UP);
            }
        } else {
            animator->play(UP);
        }
    }

    // neutrals
    void performNeutralBehavior(float deltaTime, sf::Vector2f playerPos) {
        float dist = getDistance(position, playerPos);
        
        if (dist < 20.0f) { // ttoo close
            timeNearNeutral += deltaTime;
            relation.modify(-2.0f * deltaTime); 
            
            if (timeNearNeutral > 3.0f) {
                cout << "The neutral tribe is annoyed by your presence!\n";
                relation.modify(-15.0f); 
                timeNearNeutral = 0.0f; 
            }
            animator->play(UP);
        } else {
            timeNearNeutral = 0.0f;
            performRandomWalk(deltaTime, 40.0f); 
        }
    }

    // hostiles (เพิ่มการรับค่าอารมณ์ผู้เล่นเข้ามาคำนวณ)
    void performHostileBehavior(float deltaTime, sf::Vector2f playerPos, Emotion playerEmotion) {
        if (!isHostile) {
            cout << "(Hostile) The tribe is aggressive and attacking you!\n";
            isHostile = true;
        }

        float dist = getDistance(position, playerPos);
        if (dist < 200.0f) { 
            sf::Vector2f direction = playerPos - position;
            float len = sqrt(direction.x*direction.x + direction.y*direction.y);
            if (len != 0) direction /= len;
            
            position += direction * 120.0f * deltaTime; 

            if (abs(direction.x) > abs(direction.y)) {
                if (direction.x > 0) animator->play(RIGHT);
                else animator->play(LEFT);
            } else {
                if (direction.y > 0) animator->play(DOWN);
                else animator->play(UP);
            }

            // attacks a player
            if (dist < 20.0f) { 
                wantCombat = true; // enables cutscene
                
                // pushes the entity back to prevent the cutscene to appear again
                position -= direction * 40.0f; 
            }
        } else {
            animator->play(UP);
        }
    }

    void performRandomWalk(float deltaTime, float speed) {
        randomMoveTimer -= deltaTime;
        
        if (randomMoveTimer <= 0.0f) {
            float rx = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)) - 1.0f;
            float ry = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)) - 1.0f;
            randomMoveDir = sf::Vector2f(rx, ry);

            float len = sqrt(randomMoveDir.x*randomMoveDir.x + randomMoveDir.y*randomMoveDir.y);
            if (len != 0) randomMoveDir /= len;
            else randomMoveDir = sf::Vector2f(0,0); 

            randomMoveMaxTime = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 3.0f));
            randomMoveTimer = randomMoveMaxTime;
        }

        position += randomMoveDir * speed * deltaTime;

        if (randomMoveDir.x == 0 && randomMoveDir.y == 0) {
            animator->play(UP);
        } else {
            if (abs(randomMoveDir.x) > abs(randomMoveDir.y)) {
                if (randomMoveDir.x > 0) animator->play(RIGHT);
                else animator->play(LEFT);
            } else {
                if (randomMoveDir.y > 0) animator->play(DOWN);
                else animator->play(UP);
            }
        }
    }

public:
    // constructors
    TribeEntity(float startScore, sf::Vector2f startPos, std::string textureFile) 
    : relation(startScore), position(startPos), randomMoveTimer(0), randomMoveDir(0,0), wantCombat(false) {
        timeSinceLastInteraction = 0.0f;
        timeNearNeutral = 0.0f;
        isHostile = false;

        
        currentEmotion = static_cast<Emotion>(rand() % 4);

        // assist chances
        assistChance = (rand() % 51 + 30) / 100.0f; 
        callHelpChance = (rand() % 61 + 20) / 100.0f;

        // randomize stats
        maxAttack = static_cast<float>(rand() % 31 + 20); 
        critChance = (static_cast<float>(rand() % 21 + 5)) / 100.0f;
        maxHealth = rand() % 151 + 100; 
        currentHealth = maxHealth;
        isDead = false;

        cout << "Tribe Spawned -> HP: " << maxHealth << " | MaxAtk: " << maxAttack << " | Crit: " << (critChance*100) << "% | Emotion ID: " << static_cast<int>(currentEmotion) << "\n";

        // load texture
        if (!texture.loadFromFile(textureFile)) { 
             cerr << "Error: Cannot load " << textureFile << endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(2.0f, 2.0f); 

        // animation
        animator = new AnimationHandler(sprite);
        float animSpeed = 0.1f; 
        // 38x50 sprite sheet
        animator->addAnimation({0, 6, 32, 32, 0, 0,   animSpeed}); // UP
        animator->addAnimation({1, 6, 32, 32, 0, 32,  animSpeed}); // DOWN
        animator->addAnimation({2, 6, 32, 32, 0, 64, animSpeed}); // LEFT
        animator->addAnimation({3, 6, 32, 32, 0, 96, animSpeed}); // RIGHT
        
        animator->play(UP); 
    }

    ~TribeEntity() {
        delete animator;
    }
    
    // added default parameter (ส่งอารมณ์)
    void update(float deltaTime, sf::Vector2f playerPos, Emotion playerEmotion = Emotion::Neutral) {
        if (isDead) return;

        // neglected for a moment --> decreases relationship
        timeSinceLastInteraction += deltaTime;
        if (timeSinceLastInteraction > (60.0f * 3.0f)) { 
            relation.modify(-0.5f * deltaTime); 
        }

        RelationState currentState = relation.getState();

        if (currentState != RelationState::Hostile) {
            isHostile = false;
        }

        switch (currentState) {
            case RelationState::Ally:
                performAllyBehavior(deltaTime, playerPos);
                break;
            case RelationState::Neutral:
                performNeutralBehavior(deltaTime, playerPos);
                break;
            case RelationState::Hostile:
                // player's emotion
                performHostileBehavior(deltaTime, playerPos, playerEmotion);
                break;
        }

        sprite.setPosition(position);
        animator->update(deltaTime);
    }
    
    void draw(sf::RenderWindow& window) {
        if (!isDead) {
            window.draw(sprite);
        }
    }

    bool checkAndResetCombatTrigger() {
        if (wantCombat) {
            wantCombat = false; // resets
            return true;
        }
        return false;
    }

    // getter and setter for hp info
    int getHP() const { return currentHealth; }
    int getMaxHP() const { return maxHealth; }
    int getMaxAttack() const { return static_cast<int>(maxAttack); }
    sf::Sprite getSprite() const { return sprite; }
    void setHP(int hp) { 
        currentHealth = hp; 
        if (currentHealth <= 0) {
            isDead = true; 
            cout << "The Tribe entity has been defeated.\n";
        }
    }

    
    void takeDamage(int amount) {
        if (isDead) return;

        currentHealth -= amount;
        cout << "Tribe takes " << amount << " damage! (HP: " << currentHealth << "/" << maxHealth << ")\n";

        if (currentHealth <= 0) {
            currentHealth = 0;
            isDead = true;
            cout << "The Tribe entity has been defeated!\n";
        } else {
            onAttackedByPlayer(); 
        }
    }

   
    void takeDamage(int amount, Emotion attackerEmotion) {
        float multiplier = getEmotionMultiplier(attackerEmotion, currentEmotion);
        int finalDamage = static_cast<int>(amount * multiplier);
        
        if (multiplier > 1.0f) cout << "Super effective emotion attack!\n";
        else if (multiplier < 1.0f) cout << "Not very effective emotion attack...\n";
        
        // returns to takeDamage --> decreases health
        takeDamage(finalDamage); 
    }

    void receiveItem() { 
        relation.modify(15.0f); 
        timeSinceLastInteraction = 0.0f; 
        cout << "Item Given! Tribe relationship increased.\n";
    }

    // item giving (ally)
    void giveRandomItemToPlayer() {
        if (relation.getState() == RelationState::Ally) {
            cout << "*** The Ally tribe gifted you a special item! ***\n";
        }
    }
    
    // gathering stuffs 
    void onPlayerGatherItem(sf::Vector2f playerPos) {
        if (isDead) return;

        float dist = getDistance(position, playerPos);
        RelationState state = relation.getState();

        if (state == RelationState::Ally && dist < 10.0f) {
            cout << "You safely gathered items near your Ally.\n";
        } else if (state == RelationState::Neutral) {
            // การไปเก็บของใน region ของเผ่า neutral มีโอกาสทำให้ไม่พอใจ 
            if (rand() % 100 < 60) { 
                relation.modify(-10.0f);
                cout << "The Neutral tribe did not like you stealing their resources!\n";
            }
        }
        timeSinceLastInteraction = 0.0f; 
    }

    
    void onPlayerAttacked(sf::Vector2f playerPos) {
        if (isDead) return;

        float dist = getDistance(position, playerPos);
        if (relation.getState() == RelationState::Ally && dist < 50.0f) {
            if ((rand() % 100 / 100.0f) < assistChance) {
                cout << "*** The Ally Tribe jumps in to protect you! ***\n";
            }
        }
    }

    // attack sa tribe
    void onAttackedByPlayer() {
        relation.modify(-20.0f); 
        timeSinceLastInteraction = 0.0f;
        
        if (relation.getState() == RelationState::Hostile) {
            // calls for more tribe members
            if ((rand() % 100 / 100.0f) < callHelpChance) {
                cout << "*** The tribe sounded a horn! Reinforcements are coming! ***\n";
            }
        }
    }
    
    // getter/setter funcs
    sf::Vector2f getPosition() { return position; } 
    bool isEntityDead() const { return isDead; }
    
    // เปลี่ยนอารมณ์ NPC
    Emotion getEmotion() const { return currentEmotion; }
    void setEmotion(Emotion newEmotion) { currentEmotion = newEmotion; }
    RelationState getRelationState() const { return relation.getState(); }
};