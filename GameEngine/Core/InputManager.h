#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <unordered_set>

class InputManager
{
public:
    static void beginFrame();
    static void handleEvent(const sf::Event& event);

    // True while the key is held down.
    static bool isKeyPressed(sf::Keyboard::Key key);

    // True only during the frame in which the key is released.
    static bool isKeyReleased(sf::Keyboard::Key key);

private:
    static std::unordered_set<sf::Keyboard::Key> pressedKeys;
    static std::unordered_set<sf::Keyboard::Key> releasedKeys;
};
