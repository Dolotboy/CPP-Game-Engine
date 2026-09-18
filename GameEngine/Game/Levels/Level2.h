#pragma once

#include "../../Core/GameState.h"

#include <string>

class Level2 : public GameState
{
public:
    explicit Level2();

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;
};
