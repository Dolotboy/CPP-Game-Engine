#include "EntityManager.h"

/*************************ENTITY****************************/

Entity::Entity(bool is2D, string entityName)
{
	this->is2D = is2D;
	this->entityName = entityName;
	this->entityId = EntityManager::GenerateEntityId();
}

void Entity::AddComponent()
{
}

Entity::~Entity()
{
}

/*************************ENTITY2D****************************/

Entity2D::Entity2D(string entityName, string spriteName, int width, int height) : Entity(true, entityName)
{
	this->spriteName = spriteName;
	this->width = width;
	this->height = height;
}

Entity2D::Entity2D(string entityName) : Entity(true, entityName)
{
	this->width = 0;
	this->height = 0;
}

Entity2D::~Entity2D()
{
}

/*************************ENTITY MANAGER****************************/

int EntityManager::GenerateEntityId()
{
	int id = 10;
	/*
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i].entityId > id)
		{
			id = entities[i].entityId;
		}
	}
	*/
	return id;
}

void EntityManager::CreateEntity2D(Entity2D* entity)
{
	entities2D.insert(entities2D.begin(), entity);
}

void EntityManager::CreateEntity2D(string entityName)
{

}

void EntityManager::DestroyEntity(int entityId)
{

}

void EntityManager::DestroyEntity(Entity entity)
{

}


