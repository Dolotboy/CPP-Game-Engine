#include "EntityUI.h"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace
{
void centerText(sf::Text& text, sf::Vector2f position, sf::Vector2f size)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(
        position.x + (size.x - bounds.width) / 2.0f - bounds.left,
        position.y + (size.y - bounds.height) / 2.0f - bounds.top);
}
}

EntityUI::EntityUI(const std::string& entityName, sf::Vector2f position, float z)
    : Entity2D(entityName, "", 0.0, 0.0, position.x, position.y, z, false)
{
}

const sf::Font* EntityUI::loadFont(const std::string& fontPath)
{
    if (fontPath.empty())
    {
        return nullptr;
    }

    const auto existingFont = fonts.find(fontPath);
    if (existingFont != fonts.end())
    {
        return existingFont->second.get();
    }

    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(fontPath))
    {
        return nullptr;
    }

    const sf::Font* loadedFont = font.get();
    fonts.emplace(fontPath, std::move(font));
    return loadedFont;
}

void EntityUI::addText(
    const std::string& text,
    const std::string& fontPath,
    sf::Vector2f textPosition,
    unsigned int characterSize,
    sf::Color color)
{
    const sf::Font* font = loadFont(fontPath);
    if (font == nullptr)
    {
        return;
    }

    auto label = std::make_unique<sf::Text>();
    label->setFont(*font);
    label->setString(text);
    label->setCharacterSize(characterSize);
    label->setFillColor(color);
    label->setPosition(textPosition);
    drawables.push_back(std::move(label));
}

void EntityUI::addCenteredText(
    const std::string& text,
    const std::string& fontPath,
    sf::Vector2f textPosition,
    sf::Vector2f size,
    unsigned int characterSize,
    sf::Color color)
{
    const sf::Font* font = loadFont(fontPath);
    if (font == nullptr)
    {
        return;
    }

    auto label = std::make_unique<sf::Text>();
    label->setFont(*font);
    label->setString(text);
    label->setCharacterSize(characterSize);
    label->setFillColor(color);
    centerText(*label, textPosition, size);
    drawables.push_back(std::move(label));
}

void EntityUI::addRectangle(
    sf::Vector2f rectanglePosition,
    sf::Vector2f size,
    sf::Color fillColor,
    sf::Color outlineColor,
    float outlineThickness)
{
    auto rectangle = std::make_unique<sf::RectangleShape>(size);
    rectangle->setPosition(rectanglePosition);
    rectangle->setFillColor(fillColor);
    rectangle->setOutlineColor(outlineColor);
    rectangle->setOutlineThickness(outlineThickness);
    drawables.push_back(std::move(rectangle));
}

void EntityUI::addBubbleTail(
    sf::Vector2f bubblePosition,
    sf::Vector2f size,
    sf::Color fillColor,
    sf::Color outlineColor)
{
    auto tail = std::make_unique<sf::ConvexShape>(3);
    tail->setPoint(0, sf::Vector2f(bubblePosition.x + 24.0f, bubblePosition.y + size.y));
    tail->setPoint(1, sf::Vector2f(bubblePosition.x + 48.0f, bubblePosition.y + size.y));
    tail->setPoint(2, sf::Vector2f(bubblePosition.x + 24.0f, bubblePosition.y + size.y + 22.0f));
    tail->setFillColor(fillColor);
    tail->setOutlineColor(outlineColor);
    tail->setOutlineThickness(2.0f);
    drawables.push_back(std::move(tail));
}

bool EntityUI::addImage(
    const std::string& texturePath,
    sf::Vector2f imagePosition,
    std::optional<sf::Vector2f> size)
{
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(texturePath))
    {
        return false;
    }

    auto image = std::make_unique<sf::Sprite>(*texture);
    image->setPosition(imagePosition);
    if (size.has_value())
    {
        const sf::Vector2u textureSize = texture->getSize();
        image->setScale(
            size->x / static_cast<float>(textureSize.x),
            size->y / static_cast<float>(textureSize.y));
    }

    textures.push_back(std::move(texture));
    drawables.push_back(std::move(image));
    return true;
}

void EntityUI::render(sf::RenderTarget& target)
{
    for (const std::unique_ptr<sf::Drawable>& drawable : drawables)
    {
        target.draw(*drawable);
    }
}
