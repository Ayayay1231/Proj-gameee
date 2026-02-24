#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cstdlib>

using namespace sf;
using namespace std;

class Boar {
private:
    Texture walkTexture;
    Texture idleTexture;
    Sprite sprite;
    
    // for tracking frame sizes
    Vector2u walkFrameSize;      
    Vector2u idleFrameSize;

    
    enum Direction { Down = 0, Up = 1, Left = 2, Right = 3 };
    enum State { Idle, Walking };
    
    Direction currentDirection;
    State currentState;
    unsigned int currentFrame; 
    
    // animation
    Clock animationClock;      
    float frameDuration;       
    
    // movement
    Clock aiClock;             // timer for changing actions
    float actionDuration;     
    float speed;               
    Vector2f movementVector;   

 
    
    int health;
    int maxHealth;
    int strength;
    int defense;
    bool isDead;

    Font font;
    Text floatingText;
    Clock textTimer;
    bool isShowingText;

    Clock attackTimer;       // starts cooldown time
    float attackCooldown;    

    //faces towards the player
    void setFacingDirection(float dx, float dy) {
        if (std::abs(dx) > std::abs(dy)) {
            currentDirection = (dx > 0) ? Right : Left;
        } else {
            currentDirection = (dy > 0) ? Down : Up;
        }
    }

public:
   
    Boar(float startX, float startY) : sprite(idleTexture), floatingText(font) {
        speed = 2.0f; // wandering around the map
        frameDuration = 0.1f; 
        currentFrame = 0;
        currentDirection = Down;
        currentState = Idle;

        maxHealth = 40;
        health = maxHealth;
        strength = 8;
        defense = 2;
        isDead = false;

        attackCooldown = 1.5f;

        if (font.openFromFile("textDossier/Pixelletters-RLm3.ttf") && font.openFromFile("textDossier/Pixelletters-BnJ5.ttf")) { //damage
            floatingText.setFont(font);
            floatingText.setCharacterSize(22);
            floatingText.setFillColor(sf::Color::Yellow);
            floatingText.setOutlineColor(sf::Color::Black);
            floatingText.setOutlineThickness(2.0f);
        }
        isShowingText = false;

        // textures
        if (!walkTexture.loadFromFile("SpritesTester2/Boar_Walk.png") || !idleTexture.loadFromFile("SpritesTester2/Boar_Idle.png")) {
            cerr << "ERROR::COULD NOT LOAD BOAR TEXTURES" << endl;
        }

        // calculate frame sizes
        walkFrameSize.x = walkTexture.getSize().x / 6;
        walkFrameSize.y = walkTexture.getSize().y / 4;
        
        idleFrameSize.x = idleTexture.getSize().x / 4;
        idleFrameSize.y = idleTexture.getSize().y / 4;

        sprite.setOrigin({idleFrameSize.x / 2.0f, idleFrameSize.y / 2.0f});
        sprite.setPosition({startX, startY});
        
        pickNewAction(); // random action
    }

    void takeDamage(int damageAmount) {
        if (isDead) return;

        int finalDamage = damageAmount - defense;
        if (finalDamage < 0) finalDamage = 0;
        
        health -= finalDamage;

        // floating text
        floatingText.setString("-" + to_string(finalDamage));
        isShowingText = true;
        textTimer.restart();
        
        cout << "Boar took " << finalDamage << " damage! Health: " << health << "/" << maxHealth << endl;

        if (health <= 0) {
            isDead = true;
            cout << "The Boar was defeated!" << endl;
        }
    }

    // check sprite position (for collision)
    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

    int update(sf::Vector2f playerPos) {
        if (isDead) return 0; 

        int damageToPlayer = 0;

        // calculate distance
        float dx = playerPos.x - sprite.getPosition().x;
        float dy = playerPos.y - sprite.getPosition().y;
        float distance = std::sqrt(dx*dx + dy*dy);

        // distance check
        if (distance < 50.0f) {
            
            currentState = Idle;
            movementVector = {0.f, 0.f};
            setFacingDirection(dx, dy);

            
            if (attackTimer.getElapsedTime().asSeconds() > attackCooldown) {
                damageToPlayer = strength; // sends damage
                cout << "(Hostile) The Boar bites you!\n";
                attackTimer.restart();
            }
        } 
        else if (distance < 250.0f) {
            // runs towards the player
            currentState = Walking;
            sf::Vector2f direction = { dx / distance, dy / distance };
            
            movementVector = direction * (speed * 1.5f); // runs faster
            setFacingDirection(dx, dy);
            
            sprite.move(movementVector);
        } 
        else {
            // wandering around
            if (aiClock.getElapsedTime().asSeconds() > actionDuration) {
                pickNewAction();
            }
            if (currentState == Walking) {
                Vector2f nextPos = sprite.getPosition() + movementVector;
                if (nextPos.x > 0 && nextPos.x < 1980 && nextPos.y > 0 && nextPos.y < 1080) {
                    sprite.move(movementVector);
                } else {
                    pickNewAction(); 
                }
            }
        }

        // animation update
        if (animationClock.getElapsedTime().asSeconds() > frameDuration) {
            int maxFrames = (currentState == Walking) ? 6 : 4;
            currentFrame = (currentFrame + 1) % maxFrames; 
            updateTextureRect();
            animationClock.restart(); 
        }

        if (isShowingText) {
            sf::Vector2f boarPos = sprite.getPosition();
            floatingText.setPosition({boarPos.x - 20.0f, boarPos.y - 40.0f});
            if (textTimer.getElapsedTime().asSeconds() > 1.0f) {
                isShowingText = false;
            }
        }

        return damageToPlayer; 
    }
    
   


    void draw(RenderWindow& window) {
        if (!isDead) window.draw(sprite);
        if (isShowingText) window.draw(floatingText);
    }

private:
    
    void pickNewAction() {
        if (currentState == Walking) {
            
            currentState = Idle;
            sprite.setTexture(idleTexture);
            movementVector = {0.f, 0.f}; // stop moving
            
            // rest for a random time
            actionDuration = static_cast<float>((rand() % 4) + 1); 
            
        } else {
            
            currentState = Walking;
            sprite.setTexture(walkTexture);
            
            // pick a random direction
            currentDirection = static_cast<Direction>(rand() % 4);
            
            // movementsw
            if (currentDirection == Down) movementVector = {0.f, speed};
            else if (currentDirection == Up) movementVector = {0.f, -speed};
            else if (currentDirection == Left) movementVector = {-speed, 0.f};
            else if (currentDirection == Right) movementVector = {speed, 0.f};
            
            // walks
            actionDuration = static_cast<float>((rand() % 2) + 1); 
        }

        // reset animation frame
        currentFrame = 0; 
        updateTextureRect();
        
        // restart the clock
        aiClock.restart();
    }

    
    void updateTextureRect() {
       
        Vector2u currentFrameSize = (currentState == Walking) ? walkFrameSize : idleFrameSize;

        int rectLeft = currentFrame * currentFrameSize.x;
        int rectTop = currentDirection * currentFrameSize.y;
        
        sprite.setTextureRect(IntRect(
            {rectLeft, rectTop}, 
            {static_cast<int>(currentFrameSize.x), static_cast<int>(currentFrameSize.y)}
        ));
    }
};