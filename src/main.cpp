#include <SFML/Graphics.hpp>
#include "Game.h"

int main(){
    sf::RenderWindow win(sf::VideoMode(WIN_WIDTH,WIN_HEIGHT),"FLAPPY BIRD");
    Game game(win);
    bool isAI = 1;
    game.startGameLoop(isAI); 
    return 0;
}