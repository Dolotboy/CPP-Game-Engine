#pragma once
#include <iostream>
#include <string>
#include <list>
#include "Component.h"
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

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

    void setTexture(sf::Texture texture);

    void setSprite(sf::Sprite sprite);

    void Render(sf::RenderTarget& target);

    void printInfo();


private:
    string spriteName;
    double width, height;
    sf::Texture texture;
    sf::Sprite sprite;
};

class EntityManager {
public:
    static std::vector<Entity*> entities;

    static int generateEntityId();

    static void destroyEntity(Entity* entity);

    static void destroyEntity(int entitiesId);

    static void printAllEntities();

private:
};

