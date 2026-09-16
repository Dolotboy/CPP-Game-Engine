#pragma once

#include "GameState.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <functional>
#include <string>

class MainMenu : public GameState
{
public:
    MainMenu(const std::string& fontPath, std::function<void()> playCallback);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::Font font;
    sf::RectangleShape playButton;
    sf::Text playLabel;
    std::function<void()> playCallback;
};