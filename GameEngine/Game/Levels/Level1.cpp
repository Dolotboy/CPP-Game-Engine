#include "Level1.h"

#include "../../Core/Game.h"
#include "../Abilities/Attack.h"
#include "../Abilities/Jump.h"
#include "../Abilities/Move.h"

#include <iostream>

Level1::Level1(const std::string& playerSpritePath)
{
    Player* player = EntityManager::addEntity<Player>("player", playerSpritePath, 64.0, 64.0);
    Entity2D* barrel = EntityManager::addEntity<Entity2D>(
        "barrel", "assets/sprites/barrel.png", 64.0, 64.0);
    playerId = player->entityId;
    barrelId = barrel->entityId;

    const float groundY = static_cast<float>(Game::window->getSize().y) - barrel->getSize().y;
    barrel->setPosition(220.0f, groundY);

    player->addAbility(
        Ability("attack", "Attack", AbilityControl::mouse(sf::Mouse::Button::Left),
            []() { Attack::execute(); }),
        [](const Ability& ability) { std::cout << "Added: " << ability.getSlug() << "\n"; });

    player->addAbility(
        Ability("special", "Special",
            AbilityControl::keyboard({ sf::Keyboard::LShift, sf::Keyboard::W })));

    player->addAbility(
        Ability("jump", "Jump", AbilityControl::keyboard({ sf::Keyboard::Space }),
            [this]()
            {
                if (Player* player = EntityManager::getEntity<Player>(playerId))
                    Jump::execute(*player);
            }));

    player->addAbility(
        Ability("move", "Move", AbilityControl::always(),
            [this]()
            {
                if (Player* player = EntityManager::getEntity<Player>(playerId))
                    Move::execute(*player);
            }, true));
}

void Level1::handleEvent(const sf::Event& event)
{
    (void)event;
}

void Level1::update(float deltaTime)
{
    Player* player = EntityManager::getEntity<Player>(playerId);
    Entity2D* barrel = EntityManager::getEntity<Entity2D>(barrelId);
    if (player != nullptr && barrel != nullptr)
        player->update(deltaTime, { barrel });
}

void Level1::render(sf::RenderTarget& target)
{
    if (Entity2D* barrel = EntityManager::getEntity<Entity2D>(barrelId))
        barrel->render(target);
    if (Player* player = EntityManager::getEntity<Player>(playerId))
        player->render(target);
}