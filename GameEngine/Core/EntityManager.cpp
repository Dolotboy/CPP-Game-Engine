#include "EntityManager.h"
#include "Collider2D.h"

#include <algorithm>

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error
std::vector<std::unique_ptr<Entity>> EntityManager::ownedEntities;
std::vector<Entity*> EntityManager::dontDestroyOnLoadEntities;
std::unordered_map<int, std::optional<sf::Vector2f>> EntityManager::persistentPositions;
std::unordered_map<int, sf::Vector2f> EntityManager::previousPositions;


int EntityManager::generateEntityId()
{
    int id = 0;
    for (const auto& entity : entities) {
        if (entity->entityId >= id)
        {
            id = entity->entityId + 1;
        }
    }
    return id;
}

Entity* EntityManager::getEntity(int entityId)
{
    const auto entityIt = std::find_if(
        entities.begin(),
        entities.end(),
        [entityId](const Entity* entity) { return entity->entityId == entityId; });

    return entityIt != entities.end() ? *entityIt : nullptr;
}

void EntityManager::destroyEntity(Entity* entity)
{
    if (entity == nullptr)
    {
        return;
    }

    const auto entityIt = std::find(entities.begin(), entities.end(), entity);
    if (entityIt == entities.end())
    {
        return;
    }

    entities.erase(entityIt);
    removeDontDestroyOnLoad(entity);
    persistentPositions.erase(entity->entityId);

    const auto ownedEntityIt = std::find_if(
        ownedEntities.begin(),
        ownedEntities.end(),
        [entity](const std::unique_ptr<Entity>& ownedEntity) {
            return ownedEntity.get() == entity;
        });

    if (ownedEntityIt != ownedEntities.end())
        ownedEntities.erase(ownedEntityIt);
}

void EntityManager::destroyEntity(int entitiesId)
{
    const auto entityIt = std::find_if(
        entities.begin(),
        entities.end(),
        [entitiesId](const Entity* entity) { return entity->entityId == entitiesId; });

    if (entityIt != entities.end())
    {
        destroyEntity(*entityIt);
    }
}

void EntityManager::destroyAllExcept(const std::vector<Entity*>& entitiesToKeep)
{
    const auto shouldKeep = [&entitiesToKeep](const Entity* entity) {
        const bool explicitlyKept = std::find(entitiesToKeep.begin(), entitiesToKeep.end(), entity)
            != entitiesToKeep.end();
        const bool persistent = std::find(dontDestroyOnLoadEntities.begin(),
            dontDestroyOnLoadEntities.end(), entity) != dontDestroyOnLoadEntities.end();
        return explicitlyKept || persistent;
    };

    for (auto entityIt = entities.begin(); entityIt != entities.end();)
    {
        if (shouldKeep(*entityIt))
        {
            ++entityIt;
            continue;
        }

        Entity* entity = *entityIt;
        entityIt = entities.erase(entityIt);
        persistentPositions.erase(entity->entityId);

        const auto ownedEntityIt = std::find_if(
            ownedEntities.begin(),
            ownedEntities.end(),
            [entity](const std::unique_ptr<Entity>& ownedEntity) {
                return ownedEntity.get() == entity;
            });

        if (ownedEntityIt != ownedEntities.end())
            ownedEntities.erase(ownedEntityIt);
    }

    for (Entity* entity : dontDestroyOnLoadEntities)
    {
        if (entity == nullptr)
            continue;

        const auto position = persistentPositions.find(entity->entityId);
        if (position != persistentPositions.end() && position->second.has_value())
            entity->setPosition(position->second->x, position->second->y);
    }
}

void EntityManager::dontDestroyOnLoad(
    Entity* entity,
    std::optional<sf::Vector2f> position)
{
    if (entity == nullptr)
        return;

    if (std::find(dontDestroyOnLoadEntities.begin(),
        dontDestroyOnLoadEntities.end(), entity) == dontDestroyOnLoadEntities.end())
    {
        dontDestroyOnLoadEntities.push_back(entity);
    }

    persistentPositions[entity->entityId] = position;
}

const std::vector<Entity*>& EntityManager::getDontDestroyOnLoadEntities()
{
    return dontDestroyOnLoadEntities;
}

void EntityManager::removeDontDestroyOnLoad(Entity* entity)
{
    dontDestroyOnLoadEntities.erase(
        std::remove(dontDestroyOnLoadEntities.begin(), dontDestroyOnLoadEntities.end(), entity),
        dontDestroyOnLoadEntities.end());
}

void EntityManager::renderAllEntities(sf::RenderTarget& target)
{
    for (const auto& entity : entities) {
        entity->render(target);
    }
}

void EntityManager::updateAllEntities(float deltaTime)
{
    previousPositions.clear();
    previousPositions.reserve(entities.size());

    for (Entity* entity : entities)
    {
        if (entity == nullptr)
            continue;

        previousPositions[entity->entityId] = entity->position;
        entity->update(deltaTime);
    }

    updateCollisions();
}

void EntityManager::updateCollisions()
{
    std::vector<Entity2D*> colliders;
    colliders.reserve(entities.size());

    for (Entity* entity : entities)
    {
        if (entity == nullptr)
            continue;

        if (auto* entity2D = dynamic_cast<Entity2D*>(entity))
        {
            if (entity2D->usesCollision())
                colliders.push_back(entity2D);
        }
    }

    for (size_t i = 0; i < colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < colliders.size(); ++j)
        {
            Entity2D* first = colliders[i];
            Entity2D* second = colliders[j];
            if (first == nullptr || second == nullptr)
                continue;

            const sf::FloatRect firstBounds(first->position.x, first->position.y,
                first->getSize().x, first->getSize().y);
            const sf::FloatRect secondBounds(second->position.x, second->position.y,
                second->getSize().x, second->getSize().y);

            if (firstBounds.intersects(secondBounds))
            {
                const sf::Vector2f firstPreviousPosition = previousPositions.count(first->entityId) != 0
                    ? previousPositions.at(first->entityId)
                    : first->position;
                const sf::Vector2f secondPreviousPosition = previousPositions.count(second->entityId) != 0
                    ? previousPositions.at(second->entityId)
                    : second->position;
                const bool firstMoved = firstPreviousPosition != first->position;
                const bool secondMoved = secondPreviousPosition != second->position;

                if (firstMoved && !secondMoved)
                {
                    const Collider2D::CollisionResult result =
                        Collider2D::resolve(*first, *second, firstPreviousPosition);
                    if (result.grounded)
                        first->setGrounded(true);
                }
                else if (secondMoved && !firstMoved)
                {
                    const Collider2D::CollisionResult result =
                        Collider2D::resolve(*second, *first, secondPreviousPosition);
                    if (result.grounded)
                        second->setGrounded(true);
                }
                else
                {
                    first->triggerOnCollision(*second);
                    second->triggerOnCollision(*first);
                }
            }
            else
            {
                first->triggerOnCollisionExit(*second);
                second->triggerOnCollisionExit(*first);
            }
        }
    }
}

void EntityManager::printAllEntities()
{
    for (const auto& entity : entities) {
        entity->printInfo();
    }
}
