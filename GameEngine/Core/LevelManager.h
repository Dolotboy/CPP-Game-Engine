#pragma once

#include <functional>
#include <vector>

#include "EntityManager.h"

class LevelManager
{
public:
    using LevelFactory = std::function<void(const std::vector<Entity*>&)>;

    static void changeLevel(
        const LevelFactory& instantiateLevel,
        const std::vector<Entity*>& entitiesToKeep = {});

    static void requestChangeLevel(
        const LevelFactory& instantiateLevel,
        const std::vector<Entity*>& entitiesToKeep = {});

    static void processPendingChange();

private:
    LevelManager() = delete;
};