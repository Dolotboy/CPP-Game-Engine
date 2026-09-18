#include "LevelManager.h"

namespace
{
    LevelManager::LevelFactory pendingLevelFactory;
    std::vector<Entity*> pendingEntitiesToKeep;
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
    pendingLevelFactory = instantiateLevel;
    pendingEntitiesToKeep = entitiesToKeep;
}

void LevelManager::processPendingChange()
{
    if (!pendingLevelFactory)
        return;

    LevelFactory factory = std::move(pendingLevelFactory);
    std::vector<Entity*> entitiesToKeep = std::move(pendingEntitiesToKeep);
    pendingEntitiesToKeep.clear();
    changeLevel(factory, entitiesToKeep);
}