#pragma once

#include "../Core/GameState.h"
#include "../Core/UIBuilder.h"

#include <SFML/Graphics/Font.hpp>
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
    UIBuilder ui;
    std::function<void()> playCallback;
};