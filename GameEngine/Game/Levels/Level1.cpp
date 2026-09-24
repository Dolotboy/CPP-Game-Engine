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

    Player* player = EntityManager::addEntity<Player>(
        "player", "assets/sprites/player_spritesheet.png",
        64.0, 64.0, 25.0f, groundY,
        true, // useCollision
        "Animations/player_manual.json");

	//player->setCollisionBox(16.0f, 16.0f, 32.0f, 48.0f);

    Entity2D* barrel = EntityManager::addEntity<Entity2D>(
        "barrel", "assets/sprites/placeholder.png",
        64.0, 64.0, 150.0f, groundY,
        true); // useCollision
    if (!barrel->registerAnimation("Animations/barrel.json"))
        std::cerr << "Unable to register barrel animations from Animations/barrel.json\n";
    else if (const auto neutralFrame = barrel->getAnimationFrame("Destroy", 0))
        barrel->setSprite(*neutralFrame);
    else
        std::cerr << "Unable to select frame 0 of the barrel Destroy animation\n";

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
    Entity2D* barrel = EntityManager::getEntity<Entity2D>(barrelId);
    if (barrel == nullptr)
        return;

    if (!barrelBreaking && *barrelTouchCount >= 3)
    {
        if (barrel->startAnimation("Destroy"))
        {
            barrelBreaking = true;
            barrel->setUseCollision(false);
        }
        else
        {
            std::cerr << "Unable to start the barrel Destroy animation\n";
        }
        *barrelTouchCount = 0;
    }

    if (barrelBreaking && !barrel->animation.isPlaying())
        EntityManager::destroyEntity(barrel);
}

void Level1::render(sf::RenderTarget&)
{
}
