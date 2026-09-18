#include "Collider2D.h"

#include <algorithm>

Collider2D::CollisionResult Collider2D::resolve(Entity2D& movingEntity,
	Entity2D& staticEntity, const sf::Vector2f& previousPosition)
{
	const sf::Vector2f movingSize = movingEntity.getSize();
	const sf::Vector2f staticSize = staticEntity.getSize();
	const sf::FloatRect movingBounds(movingEntity.position.x, movingEntity.position.y,
		movingSize.x, movingSize.y);
	const sf::FloatRect staticBounds(staticEntity.position.x, staticEntity.position.y,
		staticSize.x, staticSize.y);

	if (!movingBounds.intersects(staticBounds))
		return {};

	const float previousBottom = previousPosition.y + movingSize.y;
	const float previousTop = previousPosition.y;
	const float previousRight = previousPosition.x + movingSize.x;
	const float previousLeft = previousPosition.x;
	CollisionResult result;

	if (previousBottom <= staticBounds.top && movingEntity.velocity.y >= 0.0f)
	{
		movingEntity.setPosition(movingEntity.position.x,
			staticBounds.top - movingSize.y);
		movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
		result.grounded = true;
	}
	else if (previousTop >= staticBounds.top + staticBounds.height
		&& movingEntity.velocity.y < 0.0f)
	{
		movingEntity.setPosition(movingEntity.position.x,
			staticBounds.top + staticBounds.height);
		movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
	}
	else if (previousRight <= staticBounds.left)
	{
		movingEntity.setPosition(staticBounds.left - movingSize.x, movingEntity.position.y);
		movingEntity.setVelocity(0.0f, movingEntity.velocity.y);
	}
	else if (previousLeft >= staticBounds.left + staticBounds.width)
	{
		movingEntity.setPosition(staticBounds.left + staticBounds.width, movingEntity.position.y);
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
				movingDown ? staticBounds.top - movingSize.y
						   : staticBounds.top + staticBounds.height);
			movingEntity.setVelocity(movingEntity.velocity.x, 0.0f);
			result.grounded = movingDown;
		}
		else
		{
			const bool movingRight = movingEntity.velocity.x >= 0.0f;
			movingEntity.setPosition(movingRight ? staticBounds.left - movingSize.x
												 : staticBounds.left + staticBounds.width,
				movingEntity.position.y);
			movingEntity.setVelocity(0.0f, movingEntity.velocity.y);
		}
	}

	result.collided = true;
	movingEntity.triggerOnCollision(staticEntity);
	staticEntity.triggerOnCollision(movingEntity);
	return result;
}
