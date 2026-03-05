#include"camera.h"

Camera :: Camera(float zoomlvl)
:zoomlvl(zoomlvl)
{

}

sf::View Camera::GetView(sf::Vector2u windowSize)
{
    float aspect = (float)windowSize.x/(float)windowSize.y;
    sf::Vector2f size{};
    if(aspect <= 1.0f){
        size = sf::Vector2f(zoomlvl,zoomlvl/aspect);
    }else{
        size = sf:: Vector2f(zoomlvl*aspect,zoomlvl);
    }

    return sf::View(sf::Vector2f(0,0),size);
}