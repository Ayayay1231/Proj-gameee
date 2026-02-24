#pragma once // prevents including more than once
#include <algorithm> // for std::clamp

enum class RelationState {
    Hostile,Neutral,Ally     
};

class Relationship {
private:
    float score; // 0.0 - 100.0

public:
    Relationship(float startScore = 50.0f) : score(startScore) {}

    // stats
    RelationState getState() const {
        if (score >= 66.66f) {
            return RelationState::Ally;
        } else if (score >= 33.33f) {
            return RelationState::Neutral;
        } else {
            return RelationState::Hostile;
        }
    }

    // clamp (range)
    void modify(float amount) {
        score += amount;
        score = std::clamp(score, 0.0f, 100.0f);
    }

    float getScore() const { return score; }
};