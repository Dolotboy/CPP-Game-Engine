#pragma once

#include "../../Core/GameState.h"
#include "../../Core/Player.h"
#include "../../Core/LevelManager.h"

#include <string>

class Level1 : public GameState
{
public:
    Level1(LevelManager::LevelType nextLevel = typeid(void));

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;

private:
    int playerId = -1;
};