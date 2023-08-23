#include "EntityManager.h"

std::vector<Entity*> EntityManager::entities; // Need to define the static "list" otherwise you will get an unresolved external symbol error

Entity::Entity(bool is2D, string entityName)
{
    this->is2D = is2D;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();
}

Entity::Entity(string entityName)
{
    this->is2D = true;
    this->entityName = entityName;
    this->entityId = EntityManager::generateEntityId();
}

void Entity::printInfo() {
    std::cout << "Entity ID: " << entityId << std::endl;
}

Entity2D::Entity2D(string entityName, string spriteName, double width, double height) : Entity(true, entityName)
{
    this->spriteName = spriteName;
    this->width = width;
    this->height = height;
}

void Entity2D::printInfo(){
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

void EntityManager::addEntity(Entity* entity)
{
    entities.push_back(entity);
}

void EntityManager::printAllEntities()
{
    for (const auto& entity : entities) {
        entity->printInfo();
    }
}
