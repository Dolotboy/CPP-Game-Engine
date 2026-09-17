#pragma once
#include "../Core/Ability.h"
#include "../Core/EntityManager.h"
#include <functional>
#include <string>
#include <vector>

class Player : public Entity2D
{
public:
    Player(string entityName, string spriteName, double width = 32.0, double height = 32.0);

    void update(float deltaTime) override;
    bool isGrounded() const;

    void addAbility(Ability ability, std::function<void(const Ability&)> callback = {});
    bool removeAbility(const std::string& abilitySlug,
        std::function<void(const std::string&)> callback = {});
    const std::vector<Ability>& getAbilities() const;

private:
    static constexpr float gravity = 1200.0f;
    std::vector<Ability> abilities;

    float getGroundY() const;
};

