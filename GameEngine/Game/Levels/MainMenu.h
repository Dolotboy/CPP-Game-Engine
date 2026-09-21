#pragma once

#include "../../Core/GameState.h"
#include "../../Core/UIBuilder.h"

#include <functional>

class MainMenu : public GameState
{
public:
    explicit MainMenu(std::function<void()> playCallback);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) override;

private:
    std::function<void()> playCallback;
};