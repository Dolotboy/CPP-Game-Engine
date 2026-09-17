#include "EntityManager.h"

#include <algorithm>

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error
std::vector<std::unique_ptr<Entity>> EntityManager::ownedEntities;

Entity::Entity(bool is2D, string entityName)
{
    this->is2D = is2D;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();

    this->position = sf::Vector2f(0.f, 0.f);
    this->velocity = sf::Vector2f(0.f, 0.f);

    EntityManager::entities.push_back(this);
}

Entity::Entity(string entityName)
{
    this->is2D = true;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();

    this->position = sf::Vector2f(0.f, 0.f);
    this->velocity = sf::Vector2f(0.f, 0.f);

    EntityManager::entities.push_back(this);
}

void Entity::update(float deltaTime)
{
    this->position += this->velocity * deltaTime;
}

void Entity::setPosition(float x, float y)
{
    this->position = sf::Vector2f(x, y);
}

void Entity::setVelocity(float x, float y)
{
    this->velocity = sf::Vector2f(x, y);
}

void Entity::render(sf::RenderTarget& target)
{
}

void Entity::printInfo() {
    std::cout << "Entity ID: " << entityId << std::endl;
}

Entity2D::Entity2D(string entityName, string spriteName, double width, double height) : Entity(true, entityName)
{
    this->spriteName = spriteName;
    this->width = width;
    this->height = height;


    setTexture(spriteName);
    setSprite(this->texture);

    const sf::FloatRect localBounds = this->sprite.getLocalBounds();
    if (localBounds.width > 0.0f && localBounds.height > 0.0f)
    {
        this->sprite.setScale(
            static_cast<float>(width) / localBounds.width,
            static_cast<float>(height) / localBounds.height);
    }

    this->sprite.setPosition(this->position);
}

void Entity2D::setPosition(float x, float y)
{
    Entity::setPosition(x, y);
    this->sprite.setPosition(this->position);
}

void Entity2D::setTexture(string textureName)
{
    this->spriteName = textureName;

    if (!texture.loadFromFile(textureName))
    {
        std::cerr << "Unable to load texture: " << textureName << std::endl;
    }
}

void Entity2D::setSprite(const sf::Texture& texture)
{
    this->sprite.setTexture(texture);
}

sf::Vector2f Entity2D::getSize() const
{
    const sf::FloatRect bounds = sprite.getGlobalBounds();
    if (bounds.width > 0.0f && bounds.height > 0.0f)
        return sf::Vector2f(bounds.width, bounds.height);

    return sf::Vector2f(static_cast<float>(width), static_cast<float>(height));
}


void Entity2D::update(float deltaTime)
{
    Entity::update(deltaTime);
    this->sprite.setPosition(this->position);
}

void Entity2D::render(sf::RenderTarget& target)
{
    target.draw(this->sprite);
}

void Entity2D::printInfo() {
    std::cout << "Entity2D ID: " << this->entityId << ", Sprite: (" << this->spriteName << ") Width: (" << this->width << ") Height: (" << this->height << ")" << std::endl;
}

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
        return std::find(entitiesToKeep.begin(), entitiesToKeep.end(), entity)
            != entitiesToKeep.end();
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

        const auto ownedEntityIt = std::find_if(
            ownedEntities.begin(),
            ownedEntities.end(),
            [entity](const std::unique_ptr<Entity>& ownedEntity) {
                return ownedEntity.get() == entity;
            });

        if (ownedEntityIt != ownedEntities.end())
            ownedEntities.erase(ownedEntityIt);
    }
}

void EntityManager::renderAllEntities(sf::RenderTarget& target)
{
    for (const auto& entity : entities) {
        entity->render(target);
    }
}

void EntityManager::printAllEntities()
{
    for (const auto& entity : entities) {
        entity->printInfo();
    }
}
