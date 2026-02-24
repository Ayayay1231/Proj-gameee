#pragma once
#include <SFML/Graphics.hpp>
#include "relationship.h" 
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib> 

using namespace std;

class TribeEntity {
private:
    Relationship relation; 
    sf::Vector2f position;

    
    float timeSinceLastInteraction; 
    float timeNearNeutral;          
    bool hasPrintedHostile;         

    
    float assistChance;     // ally assist chance
    float callHelpChance;   // call for help

    // helper function
    float getDistance(sf::Vector2f p1, sf::Vector2f p2) {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        return sqrt(dx*dx + dy*dy);
    }

    // bahavior func
    void performAllyBehavior(float deltaTime, sf::Vector2f playerPos) {
        // walks closer to the player without decreasing relationshp
        float dist = getDistance(position, playerPos);
        if (dist > 80.0f) {
            sf::Vector2f direction = playerPos - position;
            float len = sqrt(direction.x*direction.x + direction.y*direction.y);
            if (len != 0) direction /= len;
            position += direction * 60.0f * deltaTime;
        }
    }

    void performNeutralBehavior(float deltaTime, sf::Vector2f playerPos) {
        float dist = getDistance(position, playerPos);
        
        if (dist < 10.0f) { 
            timeNearNeutral += deltaTime;
            
            // decreases relationship once getting too close
            relation.modify(-2.0f * deltaTime); 
            
            // decreases in a faster rate
            if (timeNearNeutral > 3.0f) {
                cout << "The neutral tribe is annoyed by your presence!\n";
                relation.modify(-15.0f); 
                timeNearNeutral = 0.0f; // reset
            }
        } else {
            timeNearNeutral = 0.0f; // retreat
        }
    }

    void performHostileBehavior(float deltaTime, sf::Vector2f playerPos) {
        // prints once
        if (!hasPrintedHostile) {
            cout << "(Hostile) The tribe is attacking you!\n";
            hasPrintedHostile = true;
        }

        // charges towards the player
        float dist = getDistance(position, playerPos);
        if (dist < 150.0f) {
            sf::Vector2f direction = playerPos - position;
            float len = sqrt(direction.x*direction.x + direction.y*direction.y);
            if (len != 0) direction /= len;
            position += direction * 120.0f * deltaTime; // runs fastr
        }
    }

public:
    TribeEntity(float startScore, sf::Vector2f startPos) 
    : relation(startScore), position(startPos) {
        timeSinceLastInteraction = 0.0f;
        timeNearNeutral = 0.0f;
        hasPrintedHostile = false;

        // randomizing chances
        assistChance = (rand() % 50 + 30) / 100.0f; 
        callHelpChance = (rand() % 60 + 20) / 100.0f;
    }
    
    void update(float deltaTime, sf::Vector2f playerPos) {
        // time decay
        timeSinceLastInteraction += deltaTime;
        if (timeSinceLastInteraction > 15.0f) { 
            relation.modify(-0.5f * deltaTime); // gradually decreases
        }

        RelationState currentState = relation.getState();

        // once it's not hostile
        if (currentState != RelationState::Hostile) {
            hasPrintedHostile = false;
        }

        // more behaviors
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
    
    // events

    void receiveItem() { 
        relation.modify(15.0f); 
        timeSinceLastInteraction = 0.0f; //resets time decay
        cout << "Item Given! Tribe relationship increased.\n";
    }

    
    void onPlayerGatherItem(sf::Vector2f playerPos) {
        float dist = getDistance(position, playerPos);
        RelationState state = relation.getState();

        if (state == RelationState::Ally && dist < 10.0f) {
            
            cout << "You safely gathered items near your Ally.\n";
            
        } else if (state == RelationState::Neutral) {
           
            if (rand() % 100 < 60) { 
                relation.modify(-10.0f);
                cout << "The Neutral tribe did not like you stealing their resources!\n";
            }
        }
        timeSinceLastInteraction = 0.0f; 
    }

    //gets attacked
    void onPlayerAttacked(sf::Vector2f playerPos) {
        float dist = getDistance(position, playerPos);
        
        if (relation.getState() == RelationState::Ally && dist < 50.0f) {
            
            if ((rand() % 100 / 100.0f) < assistChance) {
                cout << "*** The Ally Tribe jumps in to protect you! ***\n";
                
            }
        }
    }

    // player attacks a tribe
    void onAttackedByPlayer() {
        relation.modify(-20.0f); 
        timeSinceLastInteraction = 0.0f;
        
        if (relation.getState() == RelationState::Hostile) {
            // calls for its tribe
            if ((rand() % 100 / 100.0f) < callHelpChance) {
                cout << "*** The tribe sounded a horn! Reinforcements are coming! ***\n";
                
            }
        }
    }
    
    sf::Vector2f getPosition() { return position; } 
};