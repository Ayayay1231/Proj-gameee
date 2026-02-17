#pragma once
#include <algorithm> 

enum class RelationState {
    Hostile,
    Neutral,
    Ally     
}; 

class Relationship {
private:
    float score; 

public:
    Relationship(float startScore = 50.0f) : score(startScore) {}

    
    RelationState getState() const {
        if (score >= 66.66f) {
            return RelationState::Ally;
        } else if (score >= 33.33f) {
            return RelationState::Neutral;
        } else {
            return RelationState::Hostile;
        }
    }

    
    void modify(float amount) {
        score += amount;
        
        score = std::clamp(score, 0.0f, 100.0f);
    }

    float getScore() const { return score; }
};
