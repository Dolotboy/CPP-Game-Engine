#include "Level2.h"
#include "Level1.h"
#include "Level3.h"

#include "../../Core/Game.h"
#include "../../Core/EntityManager.h"
#include "../Abilities/Move.h"
#include "../../Core/Player.h"

#include "../Prefabs/Portal/Portal.h"

Level2::Level2()
{
    const float groundY = static_cast<float>(Game::window->getSize().y) - 64.0f;
    EntityManager::addEntity<Portal>("portal", "assets/sprites/red_portal.png", 48.0, 48.0, 100.0f, groundY, -1.0f, true, typeid(Level3));
    EntityManager::addEntity<Portal>("portal", "assets/sprites/blue_portal.png", 48.0, 48.0, 200.0f, groundY, -1.0f, true, typeid(Level1));
}

void Level2::handleEvent(const sf::Event&)
{
}

void Level2::update(float)
{
}

void Level2::render(sf::RenderTarget&)
{
}
