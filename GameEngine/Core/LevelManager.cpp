#include "LevelManager.h"

#include <unordered_map>

namespace
{
    LevelManager::LevelFactory pendingLevelFactory;
    LevelManager::LevelType pendingLevelType = typeid(void);
    std::vector<Entity*> pendingEntitiesToKeep;
    std::unordered_map<LevelManager::LevelType, LevelManager::LevelFactory> levelFactories;
}

void LevelManager::changeLevel(
    const LevelFactory& instantiateLevel,
    const std::vector<Entity*>& entitiesToKeep)
{
    EntityManager::destroyAllExcept(entitiesToKeep);

    if (instantiateLevel)
    {
        instantiateLevel(entitiesToKeep);
    }
}

void LevelManager::requestChangeLevel(
    const LevelFactory& instantiateLevel,
    const std::vector<Entity*>& entitiesToKeep)
{
    pendingLevelType = typeid(void);
    pendingLevelFactory = instantiateLevel;
    pendingEntitiesToKeep = entitiesToKeep;
}

void LevelManager::registerLevel(LevelType levelType, const LevelFactory& factory)
{
    levelFactories[levelType] = factory;
}

void LevelManager::requestChangeLevel(
    LevelType levelType,
    const std::vector<Entity*>& entitiesToKeep)
{
    pendingLevelType = levelType;
    pendingEntitiesToKeep = entitiesToKeep;
}

void LevelManager::processPendingChange()
{
    if (pendingLevelType != typeid(void))
    {
        const auto factory = levelFactories.find(pendingLevelType);
        if (factory != levelFactories.end())
            pendingLevelFactory = factory->second;
        pendingLevelType = typeid(void);
    }

    if (!pendingLevelFactory)
        return;

    LevelFactory factory = std::move(pendingLevelFactory);
    std::vector<Entity*> entitiesToKeep = std::move(pendingEntitiesToKeep);
    pendingEntitiesToKeep.clear();
    changeLevel(factory, entitiesToKeep);
}