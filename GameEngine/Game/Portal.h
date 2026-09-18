#pragma once

#include "../Core/EntityManager.h"
#include "../Core/LevelManager.h"

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

    void OnCollision(const CollisionInfo& collision) override;

private:
    LevelManager::LevelType nextLevel;
};
