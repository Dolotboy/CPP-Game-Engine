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

using namespace std;
class Entity2D;

struct CollisionInfo
{
    enum class State
    {
        Enter,
        Stay,
        Exit
    };

    State state = State::Stay;
    const Entity2D& other;
};

class Entity {
public:
    int entityId;

    sf::Vector2f position;
    sf::Vector2f velocity;

    Entity(bool is2D, string entityName, float x = 0.0f, float y = 0.0f);
    Entity(string entityName, float x = 0.0f, float y = 0.0f);
    virtual ~Entity() = default;


    virtual void update(float deltaTime);
    virtual void render(sf::RenderTarget& target);
    virtual void printInfo();

    virtual void setPosition(float x, float y);
    void setVelocity(float x, float y);

protected:
    bool is2D;
    string entityName;
};

class Entity2D : public Entity {
public:
    using CollisionHandler = std::function<void(const CollisionInfo& collision)>;

    Entity2D(string entityName, string spriteName, double width, double height,
        float x = 0.0f, float y = 0.0f, bool useCollision = false);

    bool usesCollision() const;
    void setUseCollision(bool value);

    void setPosition(float x, float y) override;
    void setTexture(string textureName);

    void setSprite(const sf::Texture& texture);
    sf::Vector2f getSize() const;

    virtual void OnCollision(const CollisionInfo& collision);
    virtual void OnCollisionEnter(const Entity2D& other);
    virtual void OnCollisionStay(const Entity2D& other);
    virtual void OnCollisionExit(const Entity2D& other);

    void setOnCollision(CollisionHandler callback);
    void triggerOnCollision(const Entity2D& other);
    void triggerOnCollisionExit(const Entity2D& other);

    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;
    virtual void setGrounded(bool grounded);

    void printInfo();


private:
    string spriteName;
    double width, height;
    sf::Texture texture;
    sf::Sprite sprite;
    CollisionHandler onCollision;
    std::unordered_set<int> collidingEntities;
    bool useCollision = false;
};

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

    static void renderAllEntities(sf::RenderTarget& target);
    static void updateAllEntities(float deltaTime);
    static void updateCollisions();

    static void printAllEntities();

private:
    static std::vector<std::unique_ptr<Entity>> ownedEntities;
    static std::unordered_map<int, sf::Vector2f> previousPositions;
};

