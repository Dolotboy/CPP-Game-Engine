#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

class GameState
{
public:
    virtual ~GameState() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
};