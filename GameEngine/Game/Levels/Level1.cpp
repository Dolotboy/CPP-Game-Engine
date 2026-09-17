#include "Level1.h"

#include "../Abilities/Attack.h"
#include "../Abilities/Jump.h"
#include "../Abilities/Move.h"

#include <iostream>

Level1::Level1(const std::string& playerSpritePath)
    : player("player", playerSpritePath, 64.0, 64.0)
{
    player.addAbility(
        Ability("attack", "Attack", AbilityControl::mouse(sf::Mouse::Button::Left),
            []() { Attack::execute(); }),
        [](const Ability& ability) { std::cout << "Added: " << ability.getSlug() << "\n"; });

    player.addAbility(
        Ability("special", "Special",
            AbilityControl::keyboard({ sf::Keyboard::LShift, sf::Keyboard::W })));

    player.addAbility(
        Ability("jump", "Jump", AbilityControl::keyboard({ sf::Keyboard::Space }),
            [this]() { Jump::execute(player); }));

    player.addAbility(
        Ability("move", "Move", AbilityControl::always(),
            [this]() { Move::execute(player); }, true));
}

void Level1::handleEvent(const sf::Event& event)
{
    (void)event;
}

void Level1::update(float deltaTime)
{
    player.update(deltaTime);
}

void Level1::render(sf::RenderTarget& target)
{
    player.render(target);
}