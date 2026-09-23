#pragma once
#include "Ability.h"
#include "Collider2D.h"
#include "EntityManager.h"
#include <functional>
#include <string>
#include <vector>

class Player : public Entity2D
{
public:
    enum class Direction
    {
        Back,
        Left,
        Front,
        Right
    };

    Player(string entityName, string spriteName, double width = 32.0, double height = 32.0,
        float x = 0.0f, float y = 0.0f, bool useCollision = true,
        const std::string& animationPath = "");

    void update(float deltaTime) override;
    void update(float deltaTime, const std::vector<Entity2D*>& obstacles);
    bool isGrounded() const;
    void setGrounded(bool grounded) override;
    Direction getDirection() const;
    const char* getDirectionName() const;
    void setDirection(Direction direction);

    void addAbility(Ability ability, std::function<void(const Ability&)> callback = {});
    bool removeAbility(const std::string& abilitySlug,
        std::function<void(const std::string&)> callback = {});
    const std::vector<Ability>& getAbilities() const;

private:
    static constexpr float gravity = 1200.0f;
    std::vector<Ability> abilities;
    bool grounded = false;
    Direction direction = Direction::Front;

    float getGroundY() const;
};
