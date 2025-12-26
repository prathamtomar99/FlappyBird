#pragma once
#include <SFML/Graphics.hpp>
#include "Globals.h"

class Pipe{
    int y_pos;
    static sf::Texture texture_down,texture_up;
    static int pipe_distance, move_speed;
public:
    Pipe(int);
    sf::Sprite sprite_up,sprite_down;
    static void loadTexture();
    void update(sf::Time&);
    float getRightBound();
    std::pair<float,float> getUpDownBounds();
};