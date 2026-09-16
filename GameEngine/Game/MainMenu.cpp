#include "MainMenu.h"

#include <utility>

MainMenu::MainMenu(std::function<void()> playCallback): playCallback(std::move(playCallback))
{
    ui.addButton(
        "Play",
        "assets/fonts/dejavu-fonts-ttf-2.37/ttf/DejaVuSans.ttf",
        sf::Vector2f(140.0f, 170.0f),
        sf::Vector2f(120.0f, 60.0f),
        [this]()
        {
            if (this->playCallback)
            {
                this->playCallback();
            }
        },
        sf::Color(55, 145, 90),
        sf::Color::White,
        28);
}

void MainMenu::handleEvent(const sf::Event& event)
{
    ui.handleEvent(event);
}

void MainMenu::update(float deltaTime)
{
    (void)deltaTime;
}

void MainMenu::render(sf::RenderTarget& target)
{
    ui.render(target);
}