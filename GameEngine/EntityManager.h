#pragma once
#include <iostream>
#include <string>
#include <list>
#include "Component.h"
#include <vector>

using namespace std;
class Entity {
public:
    int entityId;

    Entity(bool is2D, string entityName);
    Entity(string entityName);

    virtual void printInfo();

protected:
    bool is2D;
    string entityName;
};

class Entity2D : public Entity {
public:
    Entity2D(string entityName, string spriteName, double width, double height);

    void printInfo();

private:
    string spriteName;
    double width, height;
};

class EntityManager {
public:
    static std::vector<Entity*> entities;

    static int generateEntityId();

    static void addEntity(Entity* entity);

    static void printAllEntities();

private:
};

