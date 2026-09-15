#pragma once
#include "../Core/Ability.h"
#include "../Core/EntityManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <string>
#include <vector>

class Player : public Entity2D
{
public:
    Player(string entityName, string spriteName);

    void handleInput();
    void update(float deltaTime) override;

    void addAbility(Ability ability, std::function<void(const Ability&)> callback = {});
    bool removeAbility(const std::string& abilitySlug,
        std::function<void(const std::string&)> callback = {});
    const std::vector<Ability>& getAbilities() const;

private:
    float speed;
    std::vector<Ability> abilities;
};

