#include <SFML/Graphics.hpp>
#include "../Core/EntityManager.h"
#include "../Core/Game.h"
#include "Player.h"

int main()
{
    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    Player player("player", "../GameEngine/Game/assets/player.png");

    EntityManager::printAllEntities();

    myGame.start();

    return 0;
}