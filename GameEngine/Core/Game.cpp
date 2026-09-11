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
	while (window->isOpen())
	{
		window->clear();

		render();

		window->display();
	}
}

void Game::render()
{

	EntityManager::renderAllEntities(*window);

}
