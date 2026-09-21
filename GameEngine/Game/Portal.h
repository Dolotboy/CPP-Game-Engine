#pragma once

#include "../Core/LevelManager.h"
#include "../Core/UIBuilder.h"

#include <string>

class Portal : public Entity2D
{
public:
    Portal(const std::string& entityName,
        const std::string& spriteName,
        double width = 64.0,
        double height = 64.0,
        float x = 0.0f,
        float y = 0.0f,
        bool useCollision = true,
        LevelManager::LevelType nextLevel = typeid(void));
    void update(float deltaTime) override;
    void OnCollision(const CollisionInfo& collision) override;
    void OnCollisionEnter(const Entity2D& other) override;
    void OnCollisionExit(const Entity2D& other) override;

private:
    LevelManager::LevelType nextLevel;
    bool isReady = false;
};
