#pragma once

#include "../Core/GameState.h"
#include "Player.h"

#include <string>

class Level1 : public GameState
{
public:
    explicit Level1(const std::string& playerSpritePath);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;

private:
    Player player;
};