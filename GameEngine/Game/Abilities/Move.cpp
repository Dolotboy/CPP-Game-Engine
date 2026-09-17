#include "Move.h"

#include "../../Core/Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <cmath>

namespace
{
    constexpr float movementSpeed = 200.0f;
}

void Move::execute(Player& player)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moveX -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moveX += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        moveY -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        moveY += 1.0f;

    if (moveX != 0.0f || moveY != 0.0f)
    {
        const float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
    }

    player.setVelocity(moveX * movementSpeed, player.velocity.y);
}
