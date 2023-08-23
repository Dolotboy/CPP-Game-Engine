#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Game.h"

int main()
{

    Game myGame(400, 400, "Game Engine Test Window");
    myGame.init();
    myGame.start();

    Entity2D entity2("barrel", "E:\\Photos\\spongebobpatrick.jpg", 3.0, 4.0);

    EntityManager::printAllEntities();



    return 0;
}