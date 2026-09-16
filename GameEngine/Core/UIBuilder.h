#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class UIBuilder
{
public:
    using Action = std::function<void()>;

    void clear();

    void addText(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        unsigned int characterSize = 24,
        sf::Color color = sf::Color::White);

    void addButton(
        const std::string& label,
        const std::string& fontPath,
        sf::Vector2f position,
        sf::Vector2f size,
        Action action,
        sf::Color fillColor = sf::Color(55, 145, 90),
        sf::Color textColor = sf::Color::White,
        unsigned int characterSize = 24);

    void addBubble(
        const std::string& text,
        const std::string& fontPath,
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color fillColor = sf::Color(245, 245, 245),
        sf::Color textColor = sf::Color(28, 35, 48),
        unsigned int characterSize = 20);

    bool addImage(
        const std::string& texturePath,
        sf::Vector2f position,
        std::optional<sf::Vector2f> size = std::nullopt);

    void handleEvent(const sf::Event& event);
    void render(sf::RenderTarget& target) const;

private:
    const sf::Font* loadFont(const std::string& fontPath);

    struct Button
    {
        sf::FloatRect bounds;
        Action action;
    };

    std::vector<std::unique_ptr<sf::Drawable>> drawables;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::vector<std::unique_ptr<sf::Texture>> textures;
    std::vector<Button> buttons;
};
