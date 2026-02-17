#pragma once
#include <SFML/Graphics.hpp>
#include "relationship.h"
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

class TribeEntity {
private:
    Relationship relation; 
    sf::Vector2f position;

    // helper func
    float getDistance(sf::Vector2f p1, sf::Vector2f p2) {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        return std::sqrt(dx*dx + dy*dy);
    }

    // unit vec
    sf::Vector2f normalize(sf::Vector2f source) {
        float length = std::sqrt(source.x * source.x + source.y * source.y);
        if (length != 0)
            return source / length;
        else
            return source;
    }

    // behavior funcs
    void performAllyBehavior(float deltaTime, sf::Vector2f playerPos) {
        // ally (walks closer)
        float dist = getDistance(position, playerPos);
        if (dist > 60.0f) {
            sf::Vector2f direction = playerPos - position;
            direction = normalize(direction); 
            position += direction * 100.0f * deltaTime;
        }
        
        // decreases relationship when neglected
        relation.modify(-0.5f * deltaTime);
    }

    void performNeutralBehavior(float deltaTime, sf::Vector2f playerPos) {
        // neutral
        float dist = getDistance(position, playerPos);
        if (dist < 50.0f) {
            // std::cout << "(Neutral) Too close!\n";
            relation.modify(-5.0f * deltaTime);
        } else {
            relation.modify(-1.0f * deltaTime);
        }
    }

    void performHostileBehavior(float deltaTime, sf::Vector2f playerPos) {
        // hostile
        sf::Vector2f direction = playerPos - position;
        direction = normalize(direction);
        position += direction * 150.0f * deltaTime; // runs faster than allies
        // std::cout << "(Hostile) Attack!\n";
    }

public:
    // constructor
    TribeEntity(float startScore, sf::Vector2f startPos) 
    : relation(startScore), position(startPos) {
    }
    
    // updated func
    void update(float deltaTime, sf::Vector2f playerPos) {
        RelationState currentState = relation.getState();

        switch (currentState) {
            case RelationState::Ally:
                performAllyBehavior(deltaTime, playerPos);
                break;
            case RelationState::Neutral:
                performNeutralBehavior(deltaTime, playerPos);
                break;
            case RelationState::Hostile:
                performHostileBehavior(deltaTime, playerPos);
                break;
        }
    }
    
    void receiveGift(){
        relation.modify(15.0f); 
        std::cout << "Gift Received! Current Score: " << relation.getScore() << "\n";
    }
    
    // getter
    sf::Vector2f getPosition() { return position; } 
    
    
    RelationState getState() const { return relation.getState(); }
};