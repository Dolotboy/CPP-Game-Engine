#include "Level1.h"

#include "../../Core/Game.h"
#include "../Abilities/Attack.h"
#include "../Abilities/Jump.h"
#include "../Abilities/Move.h"
#include "../Portal.h"

#include <iostream>

Level1::Level1(LevelManager::LevelType nextLevel)
{
    const float groundY = static_cast<float>(Game::window->getSize().y) - 64.0f;
    Player* player = EntityManager::addEntity<Player>("player", "assets/sprites/player.png", 64.0, 64.0, 25.0f, groundY, true);
    Entity2D* barrel = EntityManager::addEntity<Entity2D>("barrel", "assets/sprites/barrel.png", 64.0, 64.0, 150.0f, groundY, true);
    EntityManager::addEntity<Portal>("portal", "assets/sprites/blue_portal.png", 48.0, 48.0, 300.0f, groundY, true, nextLevel);

    EntityManager::dontDestroyOnLoad(player);
    EntityManager::dontDestroyOnLoad(barrel);
    
    playerId = player->entityId;
    barrelId = barrel->entityId;
    const std::shared_ptr<int> touchCount = barrelTouchCount;
    barrel->setOnCollision([touchCount](const CollisionInfo& collision)
    {
        if (collision.state == CollisionInfo::State::Enter)
        {
             std::cout << "Barrel enter" << std::endl;
             ++(*touchCount);
        }

        if (collision.state == CollisionInfo::State::Exit)
            std::cout << "Barrel exit" << std::endl;

        if (dynamic_cast<const Player*>(&collision.other) != nullptr)
            std::cout << "Barrel collides with Player" << std::endl;
    });

    player->addAbility(
        Ability("attack", "Attack", AbilityControl::mouse(sf::Mouse::Button::Left),
            []() { Attack::execute(); }),
        [](const Ability& ability) { std::cout << "Added: " << ability.getSlug() << "\n"; });

    player->addAbility(
        Ability("special", "Special",
            AbilityControl::keyboard({ sf::Keyboard::LShift, sf::Keyboard::W })));

    player->addAbility(
            Ability("jump", "Jump",
                AbilityControl::keyboardAny({ sf::Keyboard::Space, sf::Keyboard::W,
                    sf::Keyboard::Up }),
            [player]()
            {
                Jump::execute(*player);
            }));

    player->addAbility(
        Ability("move", "Move", AbilityControl::always(),
            [player]()
            {
                Move::execute(*player);
            }, true));
}

void Level1::handleEvent(const sf::Event&)
{
}

void Level1::update(float)
{
    if (*barrelTouchCount >= 3)
    {
        EntityManager::destroyEntity(EntityManager::getEntity(barrelId));
        *barrelTouchCount = 0;
    }
}

void Level1::render(sf::RenderTarget&)
{
}