#include "SlotMachine.h"

#include <iostream>
#include <utility>

SlotMachine::SlotMachine(const std::string& entityName,
    const std::string& spriteName,
    double width,
    double height,
    float x,
    float y,
    bool useCollision)
    : SlotMachine(entityName, spriteName, width, height, x, y, 0.0f,
        useCollision)
{
}

SlotMachine::SlotMachine(const std::string& entityName,
    const std::string& spriteName,
    double width,
    double height,
    float x,
    float y,
    float z,
    bool useCollision)
    : Entity2D(entityName, spriteName, width, height, x, y, z, useCollision)
{
    
}

void SlotMachine::update(float deltaTime)
{
    (void)deltaTime;
}
