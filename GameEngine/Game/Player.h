#pragma once
<<<<<<< HEAD
class Player
{
=======

#include "../Core/EntityManager.h"
#include <SFML/Window/Keyboard.hpp>

class Player : public Entity2D
{
public:
    Player(string entityName, string spriteName);

    void handleInput();
    void update(float deltaTime) override;

private:
    float speed;
>>>>>>> main
};

