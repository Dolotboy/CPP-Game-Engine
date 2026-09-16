#include <SFML/Graphics.hpp>
#include "../Core/EntityManager.h"
#include "../Core/Game.h"
#include "Attack.h"
#include "Jump.h"
#include "Player.h"
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    const std::string playerSpritePath = argc > 1
        ? argv[1]
        : (std::filesystem::path(argv[0]).parent_path() / "assets/player.png").string();

    Player player("player", playerSpritePath, 64.0, 64.0);

    player.addAbility(
        Ability("attack", "Attack", AbilityControl::mouse(sf::Mouse::Button::Left),
            []() { Attack::execute(); }),
        [](const Ability& ability) { std::cout << "Added: " << ability.getSlug() << "\n"; });

    player.addAbility(
        Ability("special", "Special", AbilityControl::keyboard({ sf::Keyboard::LShift, sf::Keyboard::W })));

    player.addAbility(
        Ability("jump", "Jump", AbilityControl::keyboard({ sf::Keyboard::Space }),
            [&player]() { Jump::execute(player); }));

    EntityManager::printAllEntities();

    myGame.start();

    return 0;
}