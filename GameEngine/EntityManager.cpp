#include "EntityManager.h"

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error

Entity::Entity(bool is2D, string entityName)
{
    this->is2D = is2D;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();

    EntityManager::entities.push_back(this);
}

Entity::Entity(string entityName)
{
    this->is2D = true;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();

    EntityManager::entities.push_back(this);
}

void Entity::printInfo() {
    std::cout << "Entity ID: " << entityId << std::endl;
}

Entity2D::Entity2D(string entityName, string spriteName, double width, double height) : Entity(true, entityName)
{
    this->spriteName = spriteName;
    this->width = width;
    this->height = height;

    sf::Texture texture;
    if (!texture.loadFromFile("image.png"))
    {
        // error...
    }
}

void Entity2D::setTexture(sf::Texture texture)
{
    sf::Texture texture;
    if (texture.loadFromFile("image.png"))
    {
        // error...
    }
    this->texture = texture;
}

void Entity2D::setSprite(sf::Sprite sprite)
{
    this->sprite.setTexture(texture);
}

void Entity2D::Render(sf::RenderTarget& target)
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

void EntityManager::destroyEntity(Entity* entity)
{
    entities.push_back(entity);
}

void EntityManager::printAllEntities()
{
    for (const auto& entity : entities) {
        entity->printInfo();
    }
}
