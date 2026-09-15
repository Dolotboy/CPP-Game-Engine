#include <SFML/Graphics.hpp>
#include "../Core/EntityManager.h"
#include "../Core/Game.h"
#include "Attack.h"
#include "Player.h"
#include <iostream>

int main()
{
    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    Player player("player", "../GameEngine/Game/assets/player.png");

    player.addAbility(
        Ability("attack", "Attack", AbilityControl::mouse(sf::Mouse::Button::Left),
            []() { Attack::execute(); }),
        [](const Ability& ability) { std::cout << "Added: " << ability.getSlug() << "\n"; });

    player.addAbility(
        Ability("special", "Special",
            AbilityControl::keyboard({ sf::Keyboard::LShift, sf::Keyboard::W })));

    EntityManager::printAllEntities();

    myGame.start();

    return 0;
}