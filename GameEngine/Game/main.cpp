#include <SFML/Graphics.hpp>
#include "../Core/EntityManager.h"
#include "../Core/Game.h"

int main()
{

    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    Entity2D entity2("barrel", "../GameEngine/Game/assets/barrel.png", 3.0, 4.0);

    EntityManager::printAllEntities();

    myGame.start();

    return 0;
}