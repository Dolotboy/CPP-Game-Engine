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
    Player(string entityName, string spriteName, double width = 32.0, double height = 32.0,
        float x = 0.0f, float y = 0.0f, bool useCollision = true);

    void update(float deltaTime) override;
    void update(float deltaTime, const std::vector<Entity2D*>& obstacles);
    bool isGrounded() const;
    void setGrounded(bool grounded) override;

    void addAbility(Ability ability, std::function<void(const Ability&)> callback = {});
    bool removeAbility(const std::string& abilitySlug,
        std::function<void(const std::string&)> callback = {});
    const std::vector<Ability>& getAbilities() const;

private:
    static constexpr float gravity = 1200.0f;
    std::vector<Ability> abilities;
    bool grounded = false;

    float getGroundY() const;
};

