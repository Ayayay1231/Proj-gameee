#pragma once
#include <SFML/Graphics.hpp>

class Camera
{
    public:
    Camera(float zoomlvl = 5.0f);
    sf::View GetView(sf::Vector2u windowSize);

    float zoomlvl;
};