#include "Level3.h"

#include "../../Core/Game.h"

#include "../Prefabs/SlotMachine/SlotMachine.h"

#include <iostream>

Level3::Level3()
{
    const float groundY = static_cast<float>(Game::window->getSize().y) - 64.0f;

    EntityManager::addEntity<SlotMachine>("slotMachine", "assets/sprites/casino_slot_machine_spritesheet.png", 64.0, 64.0, 300.0f, groundY, -1.0f, true);

}

void Level3::handleEvent(const sf::Event&)
{
}

void Level3::update(float)
{

}

void Level3::render(sf::RenderTarget&)
{
}
