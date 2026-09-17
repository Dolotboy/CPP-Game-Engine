#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <functional>
#include <string>
#include <vector>

class AbilityControl
{
public:
    enum class Type
    {
        Always,
        Keyboard,
        Mouse
    };

    static AbilityControl always();
    static AbilityControl keyboard(std::vector<sf::Keyboard::Key> keys);
    static AbilityControl mouse(sf::Mouse::Button button);

    bool isPressed() const;

private:
    AbilityControl(Type controlType, std::vector<sf::Keyboard::Key> controlKeys,
        sf::Mouse::Button button);

    Type type;
    std::vector<sf::Keyboard::Key> keys;
    sf::Mouse::Button mouseButton;
};

class Ability
{
public:
    Ability(std::string abilitySlug, std::string abilityDisplayName,
        AbilityControl abilityControl, std::function<void()> abilityCallback = {},
        bool continuous = false);

    bool isTriggered();
    bool isActive() const;
    bool isContinuous() const;
    void activate() const;

    const std::string& getSlug() const;
    const std::string& getDisplayName() const;

private:
    std::string slug;
    std::string displayName;
    AbilityControl control;
    std::function<void()> callback;
    bool continuous;
    bool wasPressed = false;
};