#pragma once
#include <string>
#include <list>
#include "Component.h"
#include <vector>

using namespace std;
class Entity
{
	//Attributs
private:
	bool is2D;
	string entityName;
public:
	int entityId;
	list <Component> componentsList;

	//Methods
public:
	Entity(bool is2D, string entityName);
	void AddComponent();

	~Entity();
private:
};

class Entity2D : public Entity
{
	//Attributs
private:
	string spriteName;
	int width;
	int height;

public:

	//Methods
public:
	Entity2D(string entityName, string spriteName, int width, int height);
	Entity2D(string entityName);

	~Entity2D();

private:

};

static class EntityManager
{
//Attributs
private:
public:
	static vector<Entity2D*> entities2D;
//Methods
public:
	static int GenerateEntityId();
	static void CreateEntity2D(Entity2D* entity);
	static void CreateEntity2D(string entityName);
	static void DestroyEntity(int entityId);
	static void DestroyEntity(Entity entity);
private:
};

