#pragma once
#include "Animation.h"
#include <iostream>
#include <string>
#include <unordered_map>
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
	bool registerAnimation(const std::string& animationPath);
	bool startAnimation(const std::string& animationName);

	virtual void setPosition(float x, float y);
	void setVelocity(float x, float y);

	Animation animation;

protected:
	bool is2D;
	string entityName;
	std::unordered_map<std::string, Animation> registeredAnimations;
	std::string activeAnimationName;
};
