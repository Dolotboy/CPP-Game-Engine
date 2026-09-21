#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <optional>
#include <string>
#include <vector>

class EntityUI;

class UIBuilder
{
public:
    using Action = std::function<void()>;

    static void clear();

    static void addText(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        unsigned int characterSize = 24,
        sf::Color color = sf::Color::White);

    static void addButton(
        const std::string& label,
        const std::string& fontPath,
        sf::Vector2f position,
        sf::Vector2f size,
        Action action,
        sf::Color fillColor = sf::Color(55, 145, 90),
        sf::Color textColor = sf::Color::White,
        unsigned int characterSize = 24);

    static void addBubble(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color fillColor = sf::Color(245, 245, 245),
        sf::Color textColor = sf::Color(28, 35, 48),
        unsigned int characterSize = 20);

    static bool addImage(
        const std::string& texturePath,
        sf::Vector2f position,
        std::optional<sf::Vector2f> size = std::nullopt);

    static void handleEvent(const sf::Event& event);
private:
    static EntityUI* createElement(sf::Vector2f position);

    struct Button
    {
        sf::FloatRect bounds;
        Action action;
    };

    static std::vector<EntityUI*> elements;
    static std::vector<Button> buttons;
};
