#pragma once

#include "Component.h"
#include "EntityManager.h"

class Collider2D : public Component
{
public:
	struct CollisionResult
	{
		bool collided = false;
		bool grounded = false;
	};

	static CollisionResult resolve(Entity2D& movingEntity,
		Entity2D& staticEntity, const sf::Vector2f& previousPosition);

private:
	Collider2D() : Component("Collider2D") {}
};

