#include "Collider2D.h"

#include <algorithm>

Collider2D::CollisionResult Collider2D::resolve(Entity2D& movingEntity,
	Entity2D& staticEntity, const sf::Vector2f& previousPosition)
{
	const sf::FloatRect movingBox = movingEntity.getCollisionBox();
	const sf::FloatRect staticBox = staticEntity.getCollisionBox();
	const sf::FloatRect movingBounds(movingEntity.position.x + movingBox.left,
		movingEntity.position.y + movingBox.top, movingBox.width, movingBox.height);
	const sf::FloatRect staticBounds(staticEntity.position.x + staticBox.left,
		staticEntity.position.y + staticBox.top, staticBox.width, staticBox.height);

	if (!movingBounds.intersects(staticBounds))
		return {};

	const float previousBottom = previousPosition.y + movingBox.top + movingBox.height;
	const float previousTop = previousPosition.y + movingBox.top;
	const float previousRight = previousPosition.x + movingBox.left + movingBox.width;
	const float previousLeft = previousPosition.x + movingBox.left;
	CollisionResult result;

	if (previousBottom <= staticBounds.top && movingEntity.velocity.y >= 0.0f)
	{
		movingEntity.setPosition(movingEntity.position.x,
			staticBounds.top - movingBox.top - movingBox.height);
		movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
		result.grounded = true;
	}
	else if (previousTop >= staticBounds.top + staticBounds.height
		&& movingEntity.velocity.y < 0.0f)
	{
		movingEntity.setPosition(movingEntity.position.x,
			staticBounds.top + staticBounds.height - movingBox.top);
		movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
	}
	else if (previousRight <= staticBounds.left)
	{
		movingEntity.setPosition(staticBounds.left - movingBox.left - movingBox.width,
			movingEntity.position.y);
		movingEntity.setVelocity(0.0f, movingEntity.velocity.y);
	}
	else if (previousLeft >= staticBounds.left + staticBounds.width)
	{
		movingEntity.setPosition(staticBounds.left + staticBounds.width - movingBox.left,
			movingEntity.position.y);
		movingEntity.setVelocity(0.0f, movingEntity.velocity.y);
	}
	else
	{
		const float verticalOverlap = std::min(movingBounds.top + movingBounds.height,
			staticBounds.top + staticBounds.height) - std::max(movingBounds.top, staticBounds.top);
		const float horizontalOverlap = std::min(movingBounds.left + movingBounds.width,
			staticBounds.left + staticBounds.width) - std::max(movingBounds.left, staticBounds.left);

		if (verticalOverlap < horizontalOverlap)
		{
			const bool movingDown = movingEntity.velocity.y >= 0.0f;
			movingEntity.setPosition(movingEntity.position.x,
				movingDown ? staticBounds.top - movingBox.top - movingBox.height
							   : staticBounds.top + staticBounds.height - movingBox.top);
			movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
			result.grounded = movingDown;
		}
		else
		{
			const bool movingRight = movingEntity.velocity.x >= 0.0f;
			movingEntity.setPosition(movingRight
				? staticBounds.left - movingBox.left - movingBox.width
				: staticBounds.left + staticBounds.width - movingBox.left,
				movingEntity.position.y);
			movingEntity.setVelocity(0.0f, movingEntity.velocity.y);
		}
	}

	result.collided = true;
	movingEntity.triggerOnCollision(staticEntity);
	staticEntity.triggerOnCollision(movingEntity);
	return result;
}
