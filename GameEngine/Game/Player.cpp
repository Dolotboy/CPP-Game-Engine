#include "Player.h"
#include "../Core/Game.h"
#include <algorithm>
#include <cmath>
#include <utility>

Player::Player(string entityName, string spriteName, double width, double height)
    : Entity2D(entityName, spriteName, width, height), speed(200.0f)
{
    setPosition(100.0f, getGroundY());
    setVelocity(0.0f, 0.0f);
}

float Player::getGroundY() const
{
    if (Game::window == nullptr)
        return 0.0f;

    return static_cast<float>(Game::window->getSize().y) - getSize().y;
}

bool Player::isGrounded() const
{
    return position.y >= getGroundY() && velocity.y >= 0.0f;
}

void Player::addAbility(Ability ability, std::function<void(const Ability&)> callback)
{
    abilities.push_back(std::move(ability));

    if (callback)
        callback(abilities.back());
}

bool Player::removeAbility(const std::string& abilitySlug,
    std::function<void(const std::string&)> callback)
{
    const auto ability = std::find_if(abilities.begin(), abilities.end(),
        [&abilitySlug](const Ability& item) { return item.getSlug() == abilitySlug; });

    if (ability == abilities.end())
        return false;

    abilities.erase(ability);

    if (callback)
        callback(abilitySlug);

    return true;
}

const std::vector<Ability>& Player::getAbilities() const
{
    return abilities;
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

    setVelocity(moveX * speed, velocity.y);
}

void Player::update(float deltaTime)
{
    handleInput();

    for (Ability& ability : abilities)
    {
        if (ability.isTriggered())
            ability.activate();
    }

    setVelocity(velocity.x, velocity.y + gravity * deltaTime);
    Entity2D::update(deltaTime);

    if (position.y >= getGroundY())
    {
        setPosition(position.x, getGroundY());
        setVelocity(velocity.x, 0.0f);
    }
}
