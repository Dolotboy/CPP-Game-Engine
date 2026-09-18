#include "EntityManager.h"
#include "Collider2D.h"

#include <algorithm>

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error
std::vector<std::unique_ptr<Entity>> EntityManager::ownedEntities;
std::unordered_map<int, sf::Vector2f> EntityManager::previousPositions;

Entity::Entity(bool is2D, string entityName, float x, float y)
{
    this->is2D = is2D;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();

    this->position = sf::Vector2f(x, y);
    this->velocity = sf::Vector2f(0.f, 0.f);

    EntityManager::entities.push_back(this);
}

Entity::Entity(string entityName, float x, float y)
    : Entity(true, entityName, x, y)
{
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

Entity2D::Entity2D(string entityName, string spriteName, double width, double height,
    float x, float y, bool useCollision) : Entity(true, entityName, x, y)
{
    this->spriteName = spriteName;
    this->width = width;
    this->height = height;
    this->useCollision = useCollision;


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

bool Entity2D::usesCollision() const
{
    return this->useCollision;
}

void Entity2D::setUseCollision(bool value)
{
    this->useCollision = value;
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


void Entity2D::OnCollision(const CollisionInfo& collision)
{
    (void)collision;
}

void Entity2D::OnCollisionEnter(const Entity2D& other)
{
    (void)other;
}

void Entity2D::OnCollisionStay(const Entity2D& other)
{
    (void)other;
}

void Entity2D::OnCollisionExit(const Entity2D& other)
{
    (void)other;
}

void Entity2D::setOnCollision(CollisionHandler callback)
{
    this->onCollision = std::move(callback);
}

void Entity2D::triggerOnCollision(const Entity2D& other)
{
    const bool alreadyColliding = this->collidingEntities.count(other.entityId) != 0;
    if (!alreadyColliding)
    {
        this->collidingEntities.insert(other.entityId);
        const CollisionInfo collision{ CollisionInfo::State::Enter, other };
        this->OnCollision(collision);
        this->OnCollisionEnter(other);
        if (this->onCollision)
            this->onCollision(collision);
        return;
    }

    const CollisionInfo collision{ CollisionInfo::State::Stay, other };
    this->OnCollision(collision);
    this->OnCollisionStay(other);
    if (this->onCollision)
        this->onCollision(collision);
}

void Entity2D::triggerOnCollisionExit(const Entity2D& other)
{
    if (this->collidingEntities.erase(other.entityId) == 0)
        return;

    const CollisionInfo collision{ CollisionInfo::State::Exit, other };
    this->OnCollision(collision);
    this->OnCollisionExit(other);
    if (this->onCollision)
        this->onCollision(collision);
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

void Entity2D::setGrounded(bool grounded)
{
    (void)grounded;
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
