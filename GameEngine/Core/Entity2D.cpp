#include "Entity2D.h"
#include "EntityManager.h"

#include <iostream>

Entity2D::Entity2D(string entityName, string spriteName, double width, double height,
	float x, float y, bool useCollision) : Entity(true, entityName, x, y)
{
	this->spriteName = spriteName;
	this->width = width;
	this->height = height;
	this->collisionBox = sf::FloatRect(0.0f, 0.0f,
		static_cast<float>(width), static_cast<float>(height));
	this->useCollision = useCollision;


	setTexture(spriteName);
	setSprite(this->texture);

	const sf::FloatRect localBounds = this->sprite.getLocalBounds();
	if (localBounds.width > 0.0f && localBounds.height > 0.0f)
	{
		this->sprite.setScale(
			static_cast<float>(width) / localBounds.width,
			static_cast<float>(height) / localBounds.height);
	}

	this->sprite.setPosition(this->position);
}

bool Entity2D::usesCollision() const
{
	return this->useCollision;
}

void Entity2D::setUseCollision(bool value)
{
	this->useCollision = value;
}

void Entity2D::setPosition(float x, float y)
{
	Entity::setPosition(x, y);
	this->sprite.setPosition(this->position);
}

void Entity2D::setTexture(string textureName)
{
	this->spriteName = textureName;
	if (textureName.empty())
		return;

	if (!texture.loadFromFile(textureName))
	{
		std::cerr << "Unable to load texture: " << textureName << std::endl;
	}
}

void Entity2D::setCollisionBox(float left, float top, float boxWidth, float boxHeight)
{
	if (boxWidth <= 0.0f || boxHeight <= 0.0f)
		return;

	collisionBox = sf::FloatRect(left, top, boxWidth, boxHeight);
	hasCustomCollisionBox = true;
}

sf::FloatRect Entity2D::getCollisionBox() const
{
	if (hasCustomCollisionBox)
		return collisionBox;

	if (animation.isPlaying() || animation.isFinished())
	{
		const AnimationFrame& frame = animation.getCurrentFrame();
		if (frame.canvasSize.x > 0.0f && frame.canvasSize.y > 0.0f)
		{
			const float scaleX = static_cast<float>(width) / frame.canvasSize.x;
			const float scaleY = static_cast<float>(height) / frame.canvasSize.y;
			return sf::FloatRect(frame.offset.x * scaleX, frame.offset.y * scaleY,
				static_cast<float>(frame.textureRect.width) * scaleX,
				static_cast<float>(frame.textureRect.height) * scaleY);
		}
	}

	const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
	return sf::FloatRect(spriteBounds.left - position.x, spriteBounds.top - position.y,
		spriteBounds.width, spriteBounds.height);
}

void Entity2D::setSprite(const sf::Texture& texture)
{
	this->sprite.setTexture(texture);
}

sf::Vector2f Entity2D::getSize() const
{
	return sf::Vector2f(static_cast<float>(width), static_cast<float>(height));
}


void Entity2D::OnCollision(const CollisionInfo& collision)
{
	(void)collision;
}

void Entity2D::OnCollisionEnter(const Entity2D& other)
{
	(void)other;
}

void Entity2D::OnCollisionStay(const Entity2D& other)
{
	(void)other;
}

void Entity2D::OnCollisionExit(const Entity2D& other)
{
	(void)other;
}

void Entity2D::setOnCollision(CollisionHandler callback)
{
	this->onCollision = std::move(callback);
}

void Entity2D::triggerOnCollision(const Entity2D& other)
{
	const bool alreadyColliding = this->collidingEntities.count(other.entityId) != 0;
	if (!alreadyColliding)
	{
		this->collidingEntities.insert(other.entityId);
		const CollisionInfo collision{ CollisionInfo::State::Enter, other };
		this->OnCollision(collision);
		this->OnCollisionEnter(other);
		if (this->onCollision)
			this->onCollision(collision);
		return;
	}

	const CollisionInfo collision{ CollisionInfo::State::Stay, other };
	this->OnCollision(collision);
	this->OnCollisionStay(other);
	if (this->onCollision)
		this->onCollision(collision);
}

void Entity2D::triggerOnCollisionExit(const Entity2D& other)
{
	if (this->collidingEntities.erase(other.entityId) == 0)
		return;

	const CollisionInfo collision{ CollisionInfo::State::Exit, other };
	this->OnCollision(collision);
	this->OnCollisionExit(other);
	if (this->onCollision)
		this->onCollision(collision);
}

void Entity2D::update(float deltaTime)
{
	Entity::update(deltaTime);
	sf::Vector2f renderedPosition = this->position;
	if (animation.isPlaying() || animation.isFinished())
	{
		const sf::Texture* animationTexture = animation.getTexture();
		if (animationTexture != nullptr)
		{
			this->sprite.setTexture(*animationTexture);
			const sf::IntRect& textureRect = animation.getTextureRect();
			const AnimationFrame& frame = animation.getCurrentFrame();
			this->sprite.setTextureRect(textureRect);
			if (frame.canvasSize.x > 0.0f && frame.canvasSize.y > 0.0f)
			{
				const float scaleX = static_cast<float>(width) / frame.canvasSize.x;
				const float scaleY = static_cast<float>(height) / frame.canvasSize.y;
				this->sprite.setScale(
					scaleX, scaleY);
				renderedPosition += sf::Vector2f(frame.offset.x * scaleX,
					frame.offset.y * scaleY);
			}
		}
	}
	else if (animation.isConfigured())
	{
		this->sprite.setTexture(this->texture, true);
		const sf::FloatRect localBounds = this->sprite.getLocalBounds();
		if (localBounds.width > 0.0f && localBounds.height > 0.0f)
		{
			this->sprite.setScale(
				static_cast<float>(width) / localBounds.width,
				static_cast<float>(height) / localBounds.height);
		}
	}
	this->sprite.setPosition(renderedPosition);
}

void Entity2D::render(sf::RenderTarget& target)
{
	target.draw(this->sprite);
}

void Entity2D::setGrounded(bool grounded)
{
	(void)grounded;
}

void Entity2D::printInfo() {
	std::cout << "Entity2D ID: " << this->entityId << ", Sprite: (" << this->spriteName << ") Width: (" << this->width << ") Height: (" << this->height << ")" << std::endl;
}
