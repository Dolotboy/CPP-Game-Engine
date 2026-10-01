#include "InputManager.h"

std::unordered_set<sf::Keyboard::Key> InputManager::pressedKeys;
std::unordered_set<sf::Keyboard::Key> InputManager::releasedKeys;

void InputManager::beginFrame()
{
    releasedKeys.clear();
}

void InputManager::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        pressedKeys.insert(event.key.code);
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        pressedKeys.erase(event.key.code);
        releasedKeys.insert(event.key.code);
    }
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key)
{
    return pressedKeys.find(key) != pressedKeys.end();
}

bool InputManager::isKeyReleased(sf::Keyboard::Key key)
{
    return releasedKeys.find(key) != releasedKeys.end();
}
