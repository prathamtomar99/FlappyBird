#pragma once
#include <SFML/Graphics.hpp>
#include "Bird.h"
#include <vector>
#include <random>
#include "Globals.h"
#include "Pipe.h"
#include "BirdState.h"

class Game{
public:
    Game(sf::RenderWindow&);
    sf::RenderWindow& win;
    void startGameLoop(bool &);

private:
    sf::Texture bg_texture, ground_texture;
    sf::Sprite bg_sprite,ground_sprite1,ground_sprite2;
    const int move_speed = 270;
    void draw();
    void moveGround(sf::Time&);
    Bird bird;
    BirdState state;
    bool is_enter_pressed, run_game;
    void doProcessing(sf::Time &);
    int pipe_counter, pipe_spawn_time;
    void checkCollisions();
    std::vector<Pipe> pipes;
    std::random_device rd;
    std::uniform_int_distribution<int> dist{250,500};
    sf::Font font;
    sf::Text restart_text;
    void restartGame();
};