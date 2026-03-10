#pragma once
#include <SFML/Graphics.hpp>
#include <vector>


struct AnimationData {
    int row;            // sprite sheet
    int frameCount;     // amount of frames in 1 row
    int frameWidth;    
    int frameHeight;   
    int startX;        
    int startY;        
    float speed;        // time/frame
};

class AnimationHandler {
private:
    sf::Sprite& sprite;                 
    std::vector<AnimationData> animations; 
    int currentAnimation;              
    int currentFrame;                  
    float time;                         
    bool isLooping;                     

public:
    AnimationHandler(sf::Sprite& target) 
        : sprite(target), currentAnimation(-1), currentFrame(0), time(0.0f), isLooping(true) {}

   
    void addAnimation(AnimationData data) {
        animations.push_back(data);
    }

  
    void play(int animationId, bool loop = true) {
        if (animationId < 0 || animationId >= animations.size()) return;
        
     
        if (currentAnimation == animationId) return;

        currentAnimation = animationId;
        currentFrame = 0;
        time = 0.0f;
        isLooping = loop;
        updateTextureRect(); 
    }

    // time update (ให้ดฟรมขยับ)
    void update(float deltaTime) {
        if (currentAnimation == -1) return;

        time += deltaTime;
        const AnimationData& data = animations[currentAnimation];

        if (time >= data.speed) {
            time = 0.0f;
            currentFrame++;

            if (currentFrame >= data.frameCount) {
                if (isLooping) {
                    currentFrame = 0; // วนลูป
                } else {
                    currentFrame = data.frameCount - 1; // หยุดที่เฟรมสุดท้าย
                }
            }
            updateTextureRect();
        }
    }

private:
    
    void updateTextureRect() {
        const AnimationData& data = animations[currentAnimation];
        int left = data.startX + (currentFrame * data.frameWidth);
        int top = data.startY;
        sprite.setTextureRect(sf::IntRect(left, top, data.frameWidth, data.frameHeight));
    }
};