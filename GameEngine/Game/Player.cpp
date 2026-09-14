#include "Player.h"
<<<<<<< HEAD
=======
#include <cmath>

Player::Player(string entityName, string spriteName)
    : Entity2D(entityName, spriteName, 32.0, 32.0), speed(200.0f)
{
    setPosition(100.0f, 100.0f);
    setVelocity(0.0f, 0.0f);
}

void Player::handleInput()
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
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }

    setVelocity(moveX * speed, moveY * speed);
}

void Player::update(float deltaTime)
{
    handleInput();
    Entity2D::update(deltaTime);
}
>>>>>>> main
