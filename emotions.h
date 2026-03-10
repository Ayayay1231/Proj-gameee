#pragma once


enum class Emotion {
    Neutral,
    Happy,
    Angry,
    Sad
};


inline float getEmotionMultiplier(Emotion attacker, Emotion defender) {
    // wins -> increases dmg
    if (attacker == Emotion::Happy && defender == Emotion::Angry) return ((rand()%50 + 1) / 100.0f) + 1.0f;
    if (attacker == Emotion::Angry && defender == Emotion::Sad) return ((rand()%50 + 1) / 100.0f) + 1.0f;
    if (attacker == Emotion::Sad && defender == Emotion::Happy) return ((rand()%50 + 1) / 100.0f) + 1.0f;
    
    // loses -> decreases dmg
    if (attacker == Emotion::Happy && defender == Emotion::Sad) return 1.0f - ((rand()%50 + 1) / 100.0f);
    if (attacker == Emotion::Angry && defender == Emotion::Happy) return 1.0f - ((rand()%50 + 1) / 100.0f);
    if (attacker == Emotion::Sad && defender == Emotion::Angry) return 1.0f - ((rand()%50 + 1) / 100.0f);

    // neutral
    return 1.0f;
}