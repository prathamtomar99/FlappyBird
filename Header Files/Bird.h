#pragma once
#include <SFML/Graphics.hpp>

class Bird{
private:
    sf::Texture textures[2];
    const int gravity = 10;
    const int flap_speed = 250;
    float velocity_y;
    int anim_counter,texture_switch;
    bool should_fly;
    public:
    sf::Sprite bird_sprite;
    Bird();
    void setShouldFly(bool);
    void flapBird(sf::Time&);
    void resetBirdPosition();
    void update(sf::Time&);
    float getRightBound();
    float getVelocity();
    float getY();
};