#include "Game.h"

Game::Game(sf::RenderWindow& window):win(window),is_enter_pressed(false),run_game(true), pipe_counter(70),pipe_spawn_time(70){

    win.setFramerateLimit(80);
    bg_texture.loadFromFile("assets/bg.png");
    bg_sprite.setTexture(bg_texture);
    bg_sprite.setScale(SCALE_FACTOR,SCALE_FACTOR);
    bg_sprite.setPosition(0.f,-300.f);
    
    ground_texture.loadFromFile("assets/ground.png");
    ground_sprite1.setTexture(ground_texture);
    ground_sprite2.setTexture(ground_texture);
    ground_sprite1.setScale(SCALE_FACTOR,SCALE_FACTOR);
    ground_sprite2.setScale(SCALE_FACTOR,SCALE_FACTOR);
    ground_sprite1.setPosition(0.f,570.f);
    ground_sprite2.setPosition(ground_sprite1.getGlobalBounds().width,570.f); 

    font.loadFromFile("assets/arial.ttf");
    restart_text.setFont(font);
    restart_text.setCharacterSize(40 );
    restart_text.setFillColor(sf::Color::Black);
    restart_text.setPosition(160,650);
    restart_text.setString("Restart Game!!");

    Pipe::loadTexture();
}

void Game::startGameLoop(bool& isAI){
    sf::Clock clock;

    while (win.isOpen()){
        sf::Time dt = clock.restart();
        sf::Event event;
        
        if(isAI){
            while(win.pollEvent(event)){
                if(event.type == sf::Event::Closed){
                    win.close();
                }
            }
            if (!is_enter_pressed && run_game) {
                is_enter_pressed = true;
                bird.setShouldFly(true);
            }
            if (run_game && is_enter_pressed) {
                // int action = aiOutput(state);
                int action = 0;
                if (action == 1) {
                    bird.flapBird(dt);
                }
            }
            if (!run_game) {
                restartGame();
            }
        }
        else{
            while(win.pollEvent(event)){
                if(event.type == sf::Event::Closed){
                    win.close();
                }
                if(event.type == sf::Event::KeyPressed && run_game){
                    if(event.key.code == sf::Keyboard::Enter && !is_enter_pressed){
                        is_enter_pressed = 1;
                        bird.setShouldFly(true);
                    }
                    if(event.key.code == sf::Keyboard::Space && is_enter_pressed){
                        bird.flapBird(dt);
                    }
                }
                if(event.type == sf::Event::MouseButtonPressed && !run_game){
                    if(restart_text.getGlobalBounds().contains(event.mouseButton.x,event.mouseButton.y)){
                        restartGame();
                    }
                }
            }
        }
        
        doProcessing(dt);
        draw();
        win.display();

        if(pipes.size()>0){
            int index = 0; 
            if(pipes[0].getRightBound()<100) index = 1;
            state.next_pipe_distance_x = pipes[index].getRightBound();
            auto [a, b] = pipes[index].getUpDownBounds();
            state.next_pipe_distance_upy = a;
            state.next_pipe_distance_downy = b;
        }
        
        state.normalizePoints();
        // state.printState();   
    }
    
}

void Game::draw(){
    win.draw(bg_sprite);

    for(Pipe& pipe:pipes){
        win.draw(pipe.sprite_down);
        win.draw(pipe.sprite_up);
    }

    win.draw(ground_sprite1);
    win.draw(ground_sprite2);
    win.draw(bird.bird_sprite);

    if(!run_game){
        win.draw(restart_text);
    }
}

void Game::moveGround(sf::Time& dt){
    ground_sprite1.move(-move_speed*dt.asSeconds(),0.f);
    ground_sprite2.move(-move_speed*dt.asSeconds(),0.f);

    if(ground_sprite1.getGlobalBounds().left + ground_sprite1.getGlobalBounds().width <0){
        ground_sprite1.setPosition(ground_sprite2.getGlobalBounds().left+ground_sprite2.getGlobalBounds().width,570.f);
    }
    if(ground_sprite2.getGlobalBounds().left + ground_sprite2.getGlobalBounds().width <0){
        ground_sprite2.setPosition(ground_sprite1.getGlobalBounds().left+ground_sprite1.getGlobalBounds().width,570.f);
    }
}

void Game::doProcessing(sf::Time &dt){
    if(is_enter_pressed){
        moveGround(dt);
        if(pipe_counter>pipe_spawn_time){
            pipes.push_back(Pipe(dist(rd)));
            pipe_counter = 0;
        }
        pipe_counter++;
        for (auto it = pipes.begin(); it != pipes.end(); ) {
            it->update(dt);
            if (it->getRightBound() < 0) {
                it = pipes.erase(it);
            } else {
                ++it;
            }
        }        
        checkCollisions();
    }
    bird.update(dt);
    state.bird_velocity = bird.getVelocity();
    state.bird_y = bird.getY();
}

void Game::checkCollisions(){
    if(pipes.size()>0){
        if(pipes[0].sprite_down.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) || 
        pipes[0].sprite_up.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) || 
        bird.bird_sprite.getGlobalBounds().top>=540){
            is_enter_pressed = false;
            run_game = false;
        }

    }
}

void Game::restartGame(){
    bird.resetBirdPosition();
    bird.setShouldFly(false);
    run_game=true;
    is_enter_pressed = false;
    pipe_counter = 71;

    state.reset(bird);
    pipes.clear();
}