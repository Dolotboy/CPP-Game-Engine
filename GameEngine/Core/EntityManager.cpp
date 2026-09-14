#include "EntityManager.h"

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error

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
    this->sprite.setPosition(this->position);
}

void Entity2D::setTexture(string textureName)
{
    if (!texture.loadFromFile(textureName))
    {
        std::cerr << "Unable to load texture: " << textureName << std::endl;
    }
}

void Entity2D::setSprite(const sf::Texture& texture)
{
    this->sprite.setTexture(texture);
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

void EntityManager::destroyEntity(Entity* entity)
{
    entities.push_back(entity);
}

void EntityManager::destroyEntity(int entityId)
{
    //entities.push_back(entity);
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
