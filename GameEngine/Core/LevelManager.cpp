#include "LevelManager.h"

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