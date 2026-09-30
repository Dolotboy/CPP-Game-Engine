#pragma once

#include "../../Core/GameState.h"
#include "../../Core/Player.h"
#include "../../Core/LevelManager.h"

#include <memory>
#include <string>

class Level3 : public GameState
{
public:
    Level3();

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;

private:
};