#include "Game.h"
#include"Globals.h"
#include<sstream>
Game::Game(sf::RenderWindow& window) : win(window),
is_enter_pressed(false),
run_game(true),
start_monitoring(false),
pipe_counter(71),
pipe_spawn_time(70),
score(0)
{
	win.setFramerateLimit(60);
	bg_texture.loadFromFile("assets/bg.png");
	bg_sprite.setTexture(bg_texture);
	bg_sprite.setScale(SCALE_FACTOR, SCALE_FACTOR);
	bg_sprite.setPosition(0.f, -250.f);

	ground_texture.loadFromFile("assets/ground.png");
	ground_sprite1.setTexture(ground_texture);
	ground_sprite2.setTexture(ground_texture);

	ground_sprite1.setScale(SCALE_FACTOR, SCALE_FACTOR);
	ground_sprite2.setScale(SCALE_FACTOR, SCALE_FACTOR);

	ground_sprite1.setPosition(0, 578);
	ground_sprite2.setPosition(ground_sprite1.getGlobalBounds().width, 578);
	
	font.loadFromFile("assets/arial.ttf");
	restart_text.setFont(font);
	restart_text.setCharacterSize(48);
	restart_text.setFillColor(sf::Color::Black);
	restart_text.setPosition(150, 650);
	restart_text.setString("Restart Game!!");

	score_text.setFont(font);
	score_text.setCharacterSize(24);
	score_text.setFillColor(sf::Color::Black);
	score_text.setPosition(15, 15);
	score_text.setString("Score: 0");

	Pipe::loadTextures();

	// Networking setup
	if (ai_mode) {
		listener.listen(54000);
		client.setBlocking(true); // Synchronous mode for lock-step
	}
}

void Game::doProcessing(sf::Time& dt)
{
	if (is_enter_pressed)
	{
		moveGround(dt);

		if (pipe_counter > pipe_spawn_time)
		{
			pipes.push_back(Pipe(dist(rd)));
			pipe_counter = 0;
		}
		pipe_counter++;

		for (int i = 0;i < pipes.size();i++)
		{
			pipes[i].update(dt);
			if (pipes[i].getRightBound() < 0)
			{
				pipes.erase(pipes.begin() + i);
			}
		}

		checkCollisions();
		checkScore();
	}
	bird.update(dt);
}
void Game::startGameLoop()
{
	sf::Clock clock;

	while (win.isOpen())
	{
		// 1. Connection Phase (AI Mode)
		if (ai_mode) {
			restart_text.setString("Waiting for AI...");
			// Center text
			restart_text.setPosition(
				WIN_WIDTH/2 - restart_text.getGlobalBounds().width/2, 
				WIN_HEIGHT/2 - restart_text.getGlobalBounds().height/2
			);
			
			bool connected = false;
			while (win.isOpen() && !connected) {
				sf::Event event;
				while (win.pollEvent(event)) {
					if (event.type == sf::Event::Closed) win.close();
				}
				
				win.clear();
				win.draw(restart_text);
				win.display();

				if (listener.accept(client) == sf::Socket::Done) {
					connected = true;
					client.setBlocking(true); // Back to blocking for game sync
				}
				else {
					// Check if listener is actually listening?
					// If listen() failed in constructor, accept() returns Error immediately.
					// We should probably check that, but for now just sleep.
					sf::sleep(sf::milliseconds(100));
				}
			}
			if (!win.isOpen()) break;

			// Reset game for new connection
			restartGame();
			restart_text.setString("Restart Game!!");
			restart_text.setPosition(150, 650);
			is_enter_pressed = true; 
			bird.setShouldFly(true);
		}

		// 2. Game Loop Phase
		bool connection_lost = false;
		
		while (win.isOpen() && !connection_lost)
		{	
			sf::Time dt = clock.restart();

			// In AI mode, we ignore real time and use a fixed step to match the training environment stability
			if (ai_mode) {
				dt = sf::seconds(1.f/60.f); 
			}
			
			// AI Step
			if (ai_mode && run_game && is_enter_pressed) {
				// Send State
				sf::Packet packet;
				float birdY = bird.bird_sprite.getPosition().y;
				float velY = bird.getVelocityY();
				float pipeX = -1.0f;
				float pipeY = -1.0f;
				if (!pipes.empty()) {
					for (auto& p : pipes) {
						if (p.getRightBound() > bird.bird_sprite.getGlobalBounds().left) {
							pipeX = p.sprite_down.getPosition().x;
							pipeY = p.sprite_up.getPosition().y;
							break;
						}
					}
				}
				packet << birdY << velY << pipeX << pipeY << (sf::Int32)(run_game ? 1 : 0) << (sf::Int32)score;
				
				if (client.send(packet) != sf::Socket::Done) {
					connection_lost = true;
					break;
				}

				// Receive Action
				sf::Packet actionPacket;
				// sf::Packet handles partial receives internally if using blocking sockets, 
				// but here we are in blocking mode so it should wait for the full packet.
				// However, let's add a check.
				if (client.receive(actionPacket) != sf::Socket::Done) {
					connection_lost = true;
					break;
				}
				sf::Int32 action;
				if (actionPacket >> action) {
					if (action == 1) {
						bird.flapBird(dt);
					}
				} else {
					// Packet received but invalid size?
					connection_lost = true;
					break;
				}
			}

			sf::Event event;
			//Event Loop
			while (win.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					win.close();
				}
				if (!ai_mode) { 
					if (event.type == sf::Event::KeyPressed && run_game)
					{
						if (event.key.code == sf::Keyboard::Enter && !is_enter_pressed)
						{
							is_enter_pressed = true;
							bird.setShouldFly(true);
						}
						if (event.key.code == sf::Keyboard::Space && is_enter_pressed)
						{
							bird.flapBird(dt);
						}
					}
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !run_game)
					{
						if (restart_text.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
						{
							restartGame();
						}
					}
				}
			}
			
			// For AI, auto-restart if game over
			if (ai_mode && !run_game) {
				// Send death state
				sf::Packet packet;
				float birdY = bird.bird_sprite.getPosition().y;
				float velY = bird.getVelocityY();
				packet << birdY << velY << -1.f << -1.f << (sf::Int32)0 << (sf::Int32)score;
				if (client.send(packet) != sf::Socket::Done) {
					connection_lost = true;
					break;
				}

				// Wait for ack
				sf::Packet ackPacket;
				if (client.receive(ackPacket) != sf::Socket::Done) {
					connection_lost = true;
					break;
				}
				
				restartGame();
				is_enter_pressed = true;
				bird.setShouldFly(true);
			}

			doProcessing(dt);
			draw();
			win.display();
		}
		
		if (connection_lost) {
			client.disconnect();
			// Loop back to Connection Phase
		}
	}
}

void Game::checkCollisions()
{
	if (pipes.size() > 0)
	{
		if (pipes[0].sprite_down.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) ||
			pipes[0].sprite_up.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) ||
			bird.bird_sprite.getGlobalBounds().top >= 540)
	    {
			is_enter_pressed = false;
			run_game = false;
		}
	}
}

void Game::checkScore()
{
	if (pipes.size() > 0)
	{
		if (!start_monitoring)
		{
			if (bird.bird_sprite.getGlobalBounds().left > pipes[0].sprite_down.getGlobalBounds().left &&
				bird.getRightBound() < pipes[0].getRightBound())
			{
				start_monitoring = true;
			}
		}
		else
		{
			if (bird.bird_sprite.getGlobalBounds().left > pipes[0].getRightBound())
			{
				score++;
				score_text.setString("Score: " + toString(score));
				start_monitoring = false;
			}
		}
	}
}
void Game::draw()
{
	win.draw(bg_sprite);
	for (Pipe& pipe : pipes)
	{
		win.draw(pipe.sprite_down);
		win.draw(pipe.sprite_up);
	}
	win.draw(ground_sprite1);
	win.draw(ground_sprite2);
	win.draw(bird.bird_sprite);
	win.draw(score_text);
	if (!run_game)
	{
		win.draw(restart_text);
	}
}

void Game::moveGround(sf::Time& dt)
{
	ground_sprite1.move(-move_speed * dt.asSeconds(), 0.f);
	ground_sprite2.move(-move_speed * dt.asSeconds(), 0.f);

	if (ground_sprite1.getGlobalBounds().left + ground_sprite1.getGlobalBounds().width < 0)
	{
		ground_sprite1.setPosition(ground_sprite2.getGlobalBounds().left + ground_sprite2.getGlobalBounds().width, 578);
	}
	if (ground_sprite2.getGlobalBounds().left + ground_sprite2.getGlobalBounds().width < 0)
	{
		ground_sprite2.setPosition(ground_sprite1.getGlobalBounds().left + ground_sprite1.getGlobalBounds().width, 578);
	}

}

void Game::restartGame()
{
	bird.resetBirdPosition();
	bird.setShouldFly(false);
	run_game = true;
	is_enter_pressed = false;
	pipe_counter = 71;
	pipes.clear();
	score = 0;
	score_text.setString("Score: 0");
}

std::string Game::toString(int num)
{	
	std::stringstream ss;
	ss << num;
	return ss.str();
}

// Methods removed as they are now inlined in startGameLoop for better control context