#include "Entity2D.h"
#include "EntityManager.h"

#include <iostream>

Entity2D::Entity2D(string entityName, string spriteName, double width, double height,
	float x, float y, bool useCollision)
	: Entity2D(entityName, spriteName, width, height, x, y, 0.0f, useCollision)
{
}

Entity2D::Entity2D(string entityName, string spriteName, double width, double height,
	float x, float y, float z, bool useCollision)
	: Entity(true, entityName, x, y, z)
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

void Entity2D::setPosition(float x, float y, float z)
{
	Entity::setPosition(x, y, z);
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

	const AnimationFrame* displayedFrame = nullptr;
	if (animation.isPlaying() || animation.isFinished())
		displayedFrame = &animation.getCurrentFrame();
	else if (defaultAnimationSprite.has_value())
		displayedFrame = &defaultAnimationSprite->frame;

	if (displayedFrame != nullptr)
	{
		if (displayedFrame->canvasSize.x > 0.0f && displayedFrame->canvasSize.y > 0.0f)
		{
			const float scaleX = static_cast<float>(width) / displayedFrame->canvasSize.x;
			const float scaleY = static_cast<float>(height) / displayedFrame->canvasSize.y;
			return sf::FloatRect(displayedFrame->offset.x * scaleX,
				displayedFrame->offset.y * scaleY,
				static_cast<float>(displayedFrame->textureRect.width) * scaleX,
				static_cast<float>(displayedFrame->textureRect.height) * scaleY);
		}
	}

	const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
	return sf::FloatRect(spriteBounds.left - position.x, spriteBounds.top - position.y,
		spriteBounds.width, spriteBounds.height);
}

void Entity2D::setSprite(const sf::Texture& texture)
{
	defaultAnimationSprite.reset();
	this->sprite.setTexture(texture);
}

void Entity2D::setSprite(const AnimationSpriteFrame& frame)
{
	if (!frame.texture || frame.frame.canvasSize.x <= 0.0f ||
		frame.frame.canvasSize.y <= 0.0f || frame.frame.textureRect.width <= 0 ||
		frame.frame.textureRect.height <= 0)
		return;

	defaultAnimationSprite = frame;
	this->sprite.setTexture(*frame.texture, true);
	this->sprite.setTextureRect(frame.frame.textureRect);
	const float scaleX = static_cast<float>(width) / frame.frame.canvasSize.x;
	const float scaleY = static_cast<float>(height) / frame.frame.canvasSize.y;
	this->sprite.setScale(scaleX, scaleY);
	this->sprite.setPosition(position.x + frame.frame.offset.x * scaleX,
		position.y + frame.frame.offset.y * scaleY);
}

bool Entity2D::setSprite(const std::optional<AnimationSpriteFrame>& frame)
{
	if (!frame.has_value())
		return false;

	setSprite(*frame);
	return true;
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
	const AnimationFrame* displayedFrame = nullptr;
	const sf::Texture* displayedTexture = nullptr;
	if (animation.isPlaying() || animation.isFinished())
	{
		displayedFrame = &animation.getCurrentFrame();
		displayedTexture = animation.getTexture();
	}
	else if (defaultAnimationSprite.has_value())
	{
		displayedFrame = &defaultAnimationSprite->frame;
		displayedTexture = defaultAnimationSprite->texture.get();
	}

	if (displayedFrame != nullptr && displayedTexture != nullptr)
	{
		this->sprite.setTexture(*displayedTexture);
		this->sprite.setTextureRect(displayedFrame->textureRect);
		const float scaleX = static_cast<float>(width) / displayedFrame->canvasSize.x;
		const float scaleY = static_cast<float>(height) / displayedFrame->canvasSize.y;
		this->sprite.setScale(scaleX, scaleY);
		renderedPosition += sf::Vector2f(displayedFrame->offset.x * scaleX,
			displayedFrame->offset.y * scaleY);
	}
	else if (animation.isConfigured() && !defaultAnimationSprite.has_value())
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
