#include "SlotMachine.h"

#include "Core/Player.h"
#include "Core/InputManager.h"
#include "Core/UIBuilder.h"

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
    if (!this->registerAnimation("Animations/casino_slot_machine.json"))
        std::cerr << "Unable to register Slot Machine animations from Animations/casino_slot_machine.json\n";
    else if (const auto neutralFrame = this->getAnimationFrame("Idle", 0))
        this->setSprite(*neutralFrame);
    else
        std::cerr << "Unable to select frame 0 of the Slot Machine Idle animation\n";
}

void SlotMachine::OnCollisionEnter(const Entity2D& other)
{
    if(dynamic_cast<const Player*>(&other))
    {
        if(isPoweredOn)
            std::cout << "Slot Machine is powered on. Press Space to roll it" << std::endl;
        else
            std::cout << "Slot Machine is powered off. Press E to power it on." << std::endl;

        canPowerOn = !canPowerOn;

        UIBuilder::addImage(
            "assets/sprites/e_key.png",
            sf::Vector2f(position.x, position.y - 40.0f),
            sf::Vector2f(32.0f, 32.0f));
    }
}

void SlotMachine::OnCollisionExit(const Entity2D& other)
{
    if(dynamic_cast<const Player*>(&other))
    {
        canPowerOn = !canPowerOn;
        UIBuilder::clear();
    }
}

void SlotMachine::update(float deltaTime)
{
    (void)deltaTime;

    if (isPoweredOn && InputManager::isKeyReleased(sf::Keyboard::Space))
    {
        std::cout << "Rolling the Slot Machine!" << std::endl;
    }
    else if (!isPoweredOn && canPowerOn && InputManager::isKeyReleased(sf::Keyboard::E))
    {
        isPoweredOn = true;
        std::cout << "Slot Machine powered on!" << std::endl;
    }
    else if (isPoweredOn && canPowerOn && InputManager::isKeyReleased(sf::Keyboard::E))
    {
        isPoweredOn = false;
        std::cout << "Slot Machine powered off!" << std::endl;
    }
}
