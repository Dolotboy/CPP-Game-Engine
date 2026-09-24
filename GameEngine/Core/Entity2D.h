#pragma once
#include "Entity.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <functional>
#include <optional>
#include <unordered_set>

class Entity2D;

struct CollisionInfo
{
	enum class State
	{
		Enter,
		Stay,
		Exit
	};

	State state = State::Stay;
	const Entity2D& other;
};

class Entity2D : public Entity {
public:
	using CollisionHandler = std::function<void(const CollisionInfo& collision)>;

	Entity2D(string entityName, string spriteName, double width, double height,
		float x = 0.0f, float y = 0.0f, bool useCollision = false);

	bool usesCollision() const;
	void setUseCollision(bool value);

	void setPosition(float x, float y) override;
	void setTexture(string textureName);
	void setCollisionBox(float left, float top, float width, float height);
	sf::FloatRect getCollisionBox() const;

	void setSprite(const sf::Texture& texture);
	void setSprite(const AnimationSpriteFrame& frame);
	bool setSprite(const std::optional<AnimationSpriteFrame>& frame);
	sf::Vector2f getSize() const;

	virtual void OnCollision(const CollisionInfo& collision);
	virtual void OnCollisionEnter(const Entity2D& other);
	virtual void OnCollisionStay(const Entity2D& other);
	virtual void OnCollisionExit(const Entity2D& other);

	void setOnCollision(CollisionHandler callback);
	void triggerOnCollision(const Entity2D& other);
	void triggerOnCollisionExit(const Entity2D& other);

	void update(float deltaTime) override;
	void render(sf::RenderTarget& target) override;
	virtual void setGrounded(bool grounded);

	void printInfo();


private:
	string spriteName;
	double width, height;
	sf::FloatRect collisionBox;
	bool hasCustomCollisionBox = false;
	sf::Texture texture;
	sf::Sprite sprite;
	CollisionHandler onCollision;
	std::unordered_set<int> collidingEntities;
	std::optional<AnimationSpriteFrame> defaultAnimationSprite;
	bool useCollision = false;
};
