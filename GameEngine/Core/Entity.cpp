#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

Entity::Entity(bool is2D, string entityName, float x, float y)
{
	this->is2D = is2D;
	this->entityName = entityName;
	this->entityId = EntityManager::generateEntityId();

	this->position = sf::Vector2f(x, y);
	this->velocity = sf::Vector2f(0.f, 0.f);

	EntityManager::entities.push_back(this);
}

Entity::Entity(string entityName, float x, float y)
	: Entity(true, entityName, x, y)
{
}

void Entity::update(float deltaTime)
{
	this->position += this->velocity * deltaTime;
	this->animation.update(deltaTime);
}

void Entity::setPosition(float x, float y)
{
	this->position = sf::Vector2f(x, y);
}

void Entity::setVelocity(float x, float y)
{
	this->velocity = sf::Vector2f(x, y);
}

void Entity::render(sf::RenderTarget& target)
{
}

void Entity::printInfo() {
	std::cout << "Entity ID: " << entityId << std::endl;
}
