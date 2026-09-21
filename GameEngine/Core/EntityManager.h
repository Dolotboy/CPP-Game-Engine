#pragma once
#include <iostream>
#include <string>
#include <list>
#include "Component.h"
#include <vector>
#include <unordered_set>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <optional>

using namespace std;

#include "Entity.h"
#include "Entity2D.h"

class EntityManager {
public:
    static std::vector<Entity*> entities;

    template <typename EntityType, typename... Arguments>
    static EntityType* addEntity(Arguments&&... arguments)
    {
        static_assert(std::is_base_of<Entity, EntityType>::value,
            "EntityType must inherit from Entity");

        auto entity = std::make_unique<EntityType>(std::forward<Arguments>(arguments)...);
        EntityType* entityPointer = entity.get();
        ownedEntities.push_back(std::move(entity));
        return entityPointer;
    }

    static Entity* getEntity(int entityId);

    template <typename EntityType>
    static EntityType* getEntity(int entityId)
    {
        return dynamic_cast<EntityType*>(getEntity(entityId));
    }

    static int generateEntityId();

    static void destroyEntity(Entity* entity);

    static void destroyEntity(int entitiesId);

    static void destroyAllExcept(const std::vector<Entity*>& entitiesToKeep);

    static void dontDestroyOnLoad(Entity* entity,
        std::optional<sf::Vector2f> position = std::nullopt);
    static const std::vector<Entity*>& getDontDestroyOnLoadEntities();
    static void removeDontDestroyOnLoad(Entity* entity);

    static void renderAllEntities(sf::RenderTarget& target);
    static void updateAllEntities(float deltaTime);
    static void updateCollisions();

    static void printAllEntities();

private:
    static std::vector<std::unique_ptr<Entity>> ownedEntities;
    static std::vector<Entity*> dontDestroyOnLoadEntities;
    static std::unordered_map<int, std::optional<sf::Vector2f>> persistentPositions;
    static std::unordered_map<int, sf::Vector2f> previousPositions;
};

