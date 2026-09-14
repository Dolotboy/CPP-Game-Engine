#include <SFML/Graphics.hpp>
#include "../Core/EntityManager.h"
#include "../Core/Game.h"
<<<<<<< HEAD

int main()
{

    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    Entity2D entity2("barrel", "../GameEngine/Game/assets/barrel.png", 3.0, 4.0);
=======
#include "Player.h"

int main()
{
    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();

    Player player("player", "../GameEngine/Game/assets/player.png");
>>>>>>> main

    EntityManager::printAllEntities();

    myGame.start();

    return 0;
}