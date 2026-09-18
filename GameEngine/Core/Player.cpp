#include "Player.h"
#include "Game.h"
#include <algorithm>
#include <utility>

Player::Player(string entityName, string spriteName, double width, double height,
    float x, float y, bool useCollision)
    : Entity2D(entityName, spriteName, width, height, x, y, useCollision)
{
    if (x == 0.0f && y == 0.0f)
        setPosition(100.0f, getGroundY());
    setVelocity(0.0f, 0.0f);
    grounded = true;
}

float Player::getGroundY() const
{
    if (Game::window == nullptr)
        return 0.0f;

    return static_cast<float>(Game::window->getSize().y) - getSize().y;
}

bool Player::isGrounded() const
{
    return grounded;
}

void Player::setGrounded(bool value)
{
    grounded = value;
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
    update(deltaTime, {});
}

void Player::update(float deltaTime, const std::vector<Entity2D*>& obstacles)
{
    for (Ability& ability : abilities)
    {
        if (ability.isContinuous() ? ability.isActive() : ability.isTriggered())
            ability.activate();
    }

    const sf::Vector2f previousPosition = position;
    grounded = false;
    setVelocity(velocity.x, velocity.y + gravity * deltaTime);
    Entity2D::update(deltaTime);

    for (Entity2D* obstacle : obstacles)
    {
        if (obstacle == nullptr)
            continue;

        const Collider2D::CollisionResult collision =
            Collider2D::resolve(*this, *obstacle, previousPosition);
        grounded = grounded || collision.grounded;
    }

    if (position.y >= getGroundY())
    {
        setPosition(position.x, getGroundY());
        setVelocity(velocity.x, 0.0f);
        grounded = true;
    }
}
