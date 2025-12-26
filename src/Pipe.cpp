#include "Pipe.h"

int Pipe::pipe_distance = 150, Pipe::move_speed=400;
sf::Texture Pipe::texture_down,Pipe::texture_up;

Pipe::Pipe(int y_pos){
    this->y_pos = y_pos;
    sprite_down.setTexture(texture_down);
    sprite_up.setTexture(texture_up);

    sprite_down.setScale(SCALE_FACTOR,SCALE_FACTOR);
    sprite_up.setScale(SCALE_FACTOR,SCALE_FACTOR);

    sprite_up.setPosition(WIN_WIDTH,y_pos);
    sprite_down.setPosition(WIN_WIDTH,y_pos-sprite_down.getGlobalBounds().height-pipe_distance);
}

float Pipe::getRightBound(){
    return sprite_down.getGlobalBounds().left + sprite_down.getGlobalBounds().width;
}

std::pair<float, float> Pipe::getUpDownBounds() {
    float gapTop = sprite_down.getGlobalBounds().top + sprite_down.getGlobalBounds().height;
    float gapBottom = sprite_up.getGlobalBounds().top;
    return { gapTop, gapBottom };
}

void Pipe::update(sf::Time& dt){
    sprite_up.move(-move_speed*dt.asSeconds(),0);
    sprite_down.move(-move_speed*dt.asSeconds(),0);
}

void Pipe::loadTexture(){
    texture_down.loadFromFile("assets/pipedown.png");
    texture_up.loadFromFile("assets/pipe.png");
}
