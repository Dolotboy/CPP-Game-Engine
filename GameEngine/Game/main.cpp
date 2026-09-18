#include "../Core/Game.h"
#include "../Core/LevelManager.h"
#include "Levels/Level1.h"
#include "Levels/Level2.h"
#include "Levels/MainMenu.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <memory>
#include <string>

int main(int argc, char* argv[])
{
    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    const std::string playerSpritePath = argc > 1
        ? argv[1]
        : (std::filesystem::path(argv[0]).parent_path() / "assets/sprites/player.png").string();

    bool playRequested = false;
    std::unique_ptr<GameState> currentState = std::make_unique<MainMenu>([&playRequested]() { playRequested = true; });

    LevelManager::registerLevel(typeid(Level2),
        [&](const std::vector<Entity*>&)
        {
            currentState = std::make_unique<Level2>();
        });

    sf::Clock clock;
    while (Game::window->isOpen())
    {
        sf::Event event{};
        while (Game::window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                Game::window->close();
                continue;
            }

            currentState->handleEvent(event);
        }

        if (playRequested)
        {
            playRequested = false;
            LevelManager::changeLevel(
                [&](const std::vector<Entity*>& entitiesToKeep)
                {
                    (void)entitiesToKeep;
                    currentState = std::make_unique<Level1>(typeid(Level2));
                });
        }

        const float deltaTime = clock.restart().asSeconds();
        currentState->update(deltaTime);
        EntityManager::updateAllEntities(deltaTime);
        LevelManager::processPendingChange();

        Game::window->clear(sf::Color(28, 35, 48));
        currentState->render(*Game::window);
        EntityManager::renderAllEntities(*Game::window);
        Game::window->display();
    }

    return 0;
}