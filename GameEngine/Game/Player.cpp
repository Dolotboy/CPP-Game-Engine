#include "Player.h"
#include "../Core/Game.h"
#include <algorithm>
#include <utility>

Player::Player(string entityName, string spriteName, double width, double height)
    : Entity2D(entityName, spriteName, width, height)
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

void Player::update(float deltaTime)
{
    for (Ability& ability : abilities)
    {
        if (ability.isContinuous() ? ability.isActive() : ability.isTriggered())
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
