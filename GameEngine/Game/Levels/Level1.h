#pragma once

#include "../../Core/GameState.h"
#include "../../Core/Player.h"
#include "../../Core/LevelManager.h"

#include <memory>
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
    int barrelId = -1;
    std::shared_ptr<int> barrelTouchCount = std::make_shared<int>(0);
    bool barrelBreaking = false;
};