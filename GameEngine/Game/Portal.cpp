#include "Portal.h"

#include "../Core/Player.h"

#include <iostream>

Portal::Portal(const std::string& entityName,
    const std::string& spriteName,
    double width,
    double height,
    float x,
    float y,
    bool useCollision)
    : Entity2D(entityName, spriteName, width, height, x, y, useCollision)
{
}

void Portal::OnCollision(const CollisionInfo& collision)
{
    if (collision.state == CollisionInfo::State::Enter)
        std::cout << "Portal enter" << std::endl;

    if (collision.state == CollisionInfo::State::Exit)
        std::cout << "Portal exit" << std::endl;

    if (dynamic_cast<const Player*>(&collision.other) != nullptr)
        std::cout << "Portal collides with Player" << std::endl;
}
