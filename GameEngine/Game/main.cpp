#include "../Core/Game.h"
#include "../Core/LevelManager.h"
#include "Level1.h"
#include "MainMenu.h"

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
        : (std::filesystem::path(argv[0]).parent_path() / "assets/player.png").string();
    const std::string fontPath = argc > 2
        ? argv[2]
        : "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

    bool playRequested = false;
    std::unique_ptr<GameState> currentState = std::make_unique<MainMenu>(fontPath, [&playRequested]() { playRequested = true; });

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
                    currentState = std::make_unique<Level1>(playerSpritePath);
                });
        }

        currentState->update(clock.restart().asSeconds());

        Game::window->clear(sf::Color(28, 35, 48));
        currentState->render(*Game::window);
        Game::window->display();
    }

    return 0;
}