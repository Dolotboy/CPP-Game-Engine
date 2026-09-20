#include "Portal.h"

#include "../Core/Player.h"

#include <iostream>
#include <utility>

Portal::Portal(const std::string& entityName,
    const std::string& spriteName,
    double width,
    double height,
    float x,
    float y,
    bool useCollision,
    LevelManager::LevelType nextLevel)
    : Entity2D(entityName, spriteName, width, height, x, y, useCollision),
            nextLevel(nextLevel)
{
}

void Portal::OnCollision(const CollisionInfo& collision)
{
    if (collision.state == CollisionInfo::State::Enter)
        std::cout << "Portal enter" << std::endl;

    if (collision.state == CollisionInfo::State::Exit)
        std::cout << "Portal exit" << std::endl;

    if (collision.state == CollisionInfo::State::Enter && dynamic_cast<const Player*>(&collision.other) != nullptr)
    {
        std::cout << "Portal collides with Player" << std::endl;
    }
}

void Portal::OnCollisionEnter(const Entity2D& other)
{
    if(dynamic_cast<const Player*>(&other) != nullptr && nextLevel != typeid(void))
    {
        isReady = true;
    }
}

void Portal::OnCollisionExit(const Entity2D& other)
{
    if(dynamic_cast<const Player*>(&other) != nullptr && nextLevel != typeid(void))
    {
        isReady = false;
    }
}

void Portal::update(float deltaTime)
{
    if (isReady && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        LevelManager::requestChangeLevel(nextLevel);
}
