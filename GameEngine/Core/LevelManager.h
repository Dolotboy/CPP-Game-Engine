#pragma once

#include <functional>
#include <typeindex>
#include <vector>

#include "EntityManager.h"

class LevelManager
{
public:
    using LevelFactory = std::function<void(const std::vector<Entity*>&)>;
    using LevelType = std::type_index;

    static void changeLevel(
        const LevelFactory& instantiateLevel,
        const std::vector<Entity*>& entitiesToKeep = {});

    static void requestChangeLevel(
        const LevelFactory& instantiateLevel,
        const std::vector<Entity*>& entitiesToKeep = {});

    static void registerLevel(LevelType levelType, const LevelFactory& factory);

    static void requestChangeLevel(
        LevelType levelType,
        const std::vector<Entity*>& entitiesToKeep = {});

    static void processPendingChange();

private:
    LevelManager() = delete;
};