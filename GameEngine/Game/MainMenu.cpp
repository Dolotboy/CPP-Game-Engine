#include "MainMenu.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Mouse.hpp>
#include <utility>

MainMenu::MainMenu(const std::string& fontPath, std::function<void()> playCallback)
    : playCallback(std::move(playCallback))
{
    playButton.setSize(sf::Vector2f(120.0f, 60.0f));
    playButton.setPosition(140.0f, 170.0f);
    playButton.setFillColor(sf::Color(55, 145, 90));

    if (font.loadFromFile(fontPath))
    {
        playLabel.setFont(font);
        playLabel.setString("Play");
        playLabel.setCharacterSize(28);
        playLabel.setFillColor(sf::Color::White);

        const sf::FloatRect labelBounds = playLabel.getLocalBounds();
        playLabel.setPosition(
            playButton.getPosition().x + (playButton.getSize().x - labelBounds.width) / 2.0f
                - labelBounds.left,
            playButton.getPosition().y + (playButton.getSize().y - labelBounds.height) / 2.0f
                - labelBounds.top);
    }
}

void MainMenu::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonReleased
        && event.mouseButton.button == sf::Mouse::Left
        && playButton.getGlobalBounds().contains(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)))
    {
        if (playCallback)
        {
            playCallback();
        }
    }
}

void MainMenu::update(float deltaTime)
{
    (void)deltaTime;
}

void MainMenu::render(sf::RenderTarget& target)
{
    target.draw(playButton);
    if (!playLabel.getString().isEmpty())
    {
        target.draw(playLabel);
    }
}