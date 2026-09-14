#include "Game.h"

sf::RenderWindow* Game::window = nullptr; // Initialize to nullptr

Game::Game(int width, int height, string titleScreen)
{
	window = new sf::RenderWindow(sf::VideoMode(width, height), titleScreen);
	window->setVerticalSyncEnabled(true);
}

Game::~Game()
{
	delete window;
}

void Game::init()
{
	window->setFramerateLimit(FPS);
}

void Game::start()
{
	update();
}

void Game::update()
{
	sf::Clock clock;

	while (window->isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		for (auto& entity : EntityManager::entities)
		{
			entity->update(deltaTime);
		}

		window->clear();
		render();
		window->display();
	}
}

void Game::render()
{

	EntityManager::renderAllEntities(*window);

}
