#pragma once
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <optional>
#include "EntityManager.h"

using namespace std;
class Game
{
public:
	Game(int width, int height, string titleScreen);
	~Game();

	void init();
	void start();
	void update();
	void render();
private:
	const unsigned int FPS = 60;
protected:
public:
	static sf::RenderWindow* window;
private:
protected:
};


