#pragma once

#include "Entity2D.h"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class EntityUI : public Entity2D
{
public:
    EntityUI(
        const std::string& entityName,
        sf::Vector2f position = sf::Vector2f(0.0f, 0.0f));

    void addText(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        unsigned int characterSize,
        sf::Color color);

    void addCenteredText(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        sf::Vector2f size,
        unsigned int characterSize,
        sf::Color color);

    void addRectangle(
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color fillColor,
        sf::Color outlineColor = sf::Color::Transparent,
        float outlineThickness = 0.0f);

    void addBubbleTail(
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color fillColor,
        sf::Color outlineColor);

    bool addImage(
        const std::string& texturePath,
        sf::Vector2f position,
        std::optional<sf::Vector2f> size = std::nullopt);

    void render(sf::RenderTarget& target) override;

private:
    const sf::Font* loadFont(const std::string& fontPath);

    std::vector<std::unique_ptr<sf::Drawable>> drawables;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::vector<std::unique_ptr<sf::Texture>> textures;
};
