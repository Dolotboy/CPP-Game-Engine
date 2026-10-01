#pragma once

#include "Core/Entity2D.h"

#include <string>

class SlotMachine : public Entity2D
{
public:
    SlotMachine (const std::string& entityName,
        const std::string& spriteName,
        double width = 64.0,
        double height = 64.0,
        float x = 0.0f,
        float y = 0.0f,
        bool useCollision = true);
    SlotMachine(const std::string& entityName,
        const std::string& spriteName,
        double width,
        double height,
        float x,
        float y,
        float z,
        bool useCollision = true);
    void update(float deltaTime) override;
    void OnCollisionEnter(const Entity2D& other) override;
    void OnCollisionExit(const Entity2D& other) override;
    void DisplayActionKey();

private:
    bool playerInRange = false;
    bool isPoweredOn = false;
};
