#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

using namespace std;

class Entity {
public:
	int entityId;

	sf::Vector2f position;
	sf::Vector2f velocity;

	Entity(bool is2D, string entityName, float x = 0.0f, float y = 0.0f);
	Entity(string entityName, float x = 0.0f, float y = 0.0f);
	virtual ~Entity() = default;

	virtual void update(float deltaTime);
	virtual void render(sf::RenderTarget& target);
	virtual void printInfo();

	virtual void setPosition(float x, float y);
	void setVelocity(float x, float y);

protected:
	bool is2D;
	string entityName;
};
