#include "UIBuilder.h"

#include "EntityManager.h"
#include "EntityUI.h"

#include <utility>

std::vector<EntityUI*> UIBuilder::elements;
std::vector<UIBuilder::Button> UIBuilder::buttons;

EntityUI* UIBuilder::createElement(sf::Vector2f position)
{
    EntityUI* element = EntityManager::addEntity<EntityUI>("EntityUI", position);
    elements.push_back(element);
    return element;
}

void UIBuilder::clear()
{
    std::vector<EntityUI*> elementsToDestroy = std::move(elements);
    elements.clear();
    buttons.clear();

    for (EntityUI* element : elementsToDestroy)
    {
        EntityManager::destroyEntity(element);
    }
}

void UIBuilder::addText(
    const std::string& text,
    const std::string& fontPath,
    sf::Vector2f position,
    unsigned int characterSize,
    sf::Color color)
{
    createElement(position)->addText(text, fontPath, position, characterSize, color);
}

void UIBuilder::addButton(
    const std::string& label,
    const std::string& fontPath,
    sf::Vector2f position,
    sf::Vector2f size,
    Action action,
    sf::Color fillColor,
    sf::Color textColor,
    unsigned int characterSize)
{
    EntityUI* element = createElement(position);
    element->addRectangle(position, size, fillColor);
    element->addCenteredText(label, fontPath, position, size, characterSize, textColor);
    buttons.push_back({sf::FloatRect(position, size), std::move(action)});
}

void UIBuilder::addBubble(
    const std::string& text,
    const std::string& fontPath,
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Color fillColor,
    sf::Color textColor,
    unsigned int characterSize)
{
    EntityUI* element = createElement(position);
    element->addRectangle(position, size, fillColor, textColor, 2.0f);
    element->addBubbleTail(position, size, fillColor, textColor);
    element->addText(
        text,
        fontPath,
        sf::Vector2f(position.x + 14.0f, position.y + 10.0f),
        characterSize,
        textColor);
}

bool UIBuilder::addImage(
    const std::string& texturePath,
    sf::Vector2f position,
    std::optional<sf::Vector2f> size)
{
    EntityUI* element = createElement(position);
    if (element->addImage(texturePath, position, size))
    {
        return true;
    }

    EntityManager::destroyEntity(element);
    elements.pop_back();
    return false;
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
