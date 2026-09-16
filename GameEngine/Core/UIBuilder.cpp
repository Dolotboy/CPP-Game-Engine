#include "UIBuilder.h"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>
#include <utility>

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

UIBuilder::UIBuilder(const sf::Font* font)
    : font(font)
{
}

void UIBuilder::setFont(const sf::Font& newFont)
{
    font = &newFont;
}

void UIBuilder::clear()
{
    drawables.clear();
    textures.clear();
    buttons.clear();
}

void UIBuilder::addText(
    const std::string& text,
    sf::Vector2f position,
    unsigned int characterSize,
    sf::Color color)
{
    if (font == nullptr)
    {
        return;
    }

    auto label = std::make_unique<sf::Text>();
    label->setFont(*font);
    label->setString(text);
    label->setCharacterSize(characterSize);
    label->setFillColor(color);
    label->setPosition(position);
    drawables.push_back(std::move(label));
}

void UIBuilder::addButton(
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size,
    Action action,
    sf::Color fillColor,
    sf::Color textColor,
    unsigned int characterSize)
{
    auto button = std::make_unique<sf::RectangleShape>(size);
    button->setPosition(position);
    button->setFillColor(fillColor);
    drawables.push_back(std::move(button));

    if (font != nullptr)
    {
        auto buttonLabel = std::make_unique<sf::Text>();
        buttonLabel->setFont(*font);
        buttonLabel->setString(label);
        buttonLabel->setCharacterSize(characterSize);
        buttonLabel->setFillColor(textColor);
        centerText(*buttonLabel, position, size);
        drawables.push_back(std::move(buttonLabel));
    }

    buttons.push_back({sf::FloatRect(position, size), std::move(action)});
}

void UIBuilder::addBubble(
    const std::string& text,
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Color fillColor,
    sf::Color textColor,
    unsigned int characterSize)
{
    auto body = std::make_unique<sf::RectangleShape>(size);
    body->setPosition(position);
    body->setFillColor(fillColor);
    body->setOutlineColor(textColor);
    body->setOutlineThickness(2.0f);
    drawables.push_back(std::move(body));

    auto tail = std::make_unique<sf::ConvexShape>(3);
    tail->setPoint(0, sf::Vector2f(position.x + 24.0f, position.y + size.y));
    tail->setPoint(1, sf::Vector2f(position.x + 48.0f, position.y + size.y));
    tail->setPoint(2, sf::Vector2f(position.x + 24.0f, position.y + size.y + 22.0f));
    tail->setFillColor(fillColor);
    tail->setOutlineColor(textColor);
    tail->setOutlineThickness(2.0f);
    drawables.push_back(std::move(tail));

    if (font != nullptr)
    {
        auto bubbleText = std::make_unique<sf::Text>();
        bubbleText->setFont(*font);
        bubbleText->setString(text);
        bubbleText->setCharacterSize(characterSize);
        bubbleText->setFillColor(textColor);
        bubbleText->setPosition(position.x + 14.0f, position.y + 10.0f);
        drawables.push_back(std::move(bubbleText));
    }
}

bool UIBuilder::addImage(
    const std::string& texturePath,
    sf::Vector2f position,
    std::optional<sf::Vector2f> size)
{
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(texturePath))
    {
        return false;
    }

    auto image = std::make_unique<sf::Sprite>(*texture);
    image->setPosition(position);
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

void UIBuilder::handleEvent(const sf::Event& event)
{
    if (event.type != sf::Event::MouseButtonReleased
        || event.mouseButton.button != sf::Mouse::Left)
    {
        return;
    }

    const sf::Vector2f mousePosition(
        static_cast<float>(event.mouseButton.x),
        static_cast<float>(event.mouseButton.y));

    for (Button& button : buttons)
    {
        if (button.bounds.contains(mousePosition) && button.action)
        {
            button.action();
        }
    }
}

void UIBuilder::render(sf::RenderTarget& target) const
{
    for (const std::unique_ptr<sf::Drawable>& drawable : drawables)
    {
        target.draw(*drawable);
    }
}
