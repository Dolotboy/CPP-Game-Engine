#include "Ability.h"

#include <utility>

AbilityControl AbilityControl::keyboard(std::vector<sf::Keyboard::Key> keys)
{
    return AbilityControl(Type::Keyboard, std::move(keys), sf::Mouse::Button::Left);
}

AbilityControl AbilityControl::mouse(sf::Mouse::Button button)
{
    return AbilityControl(Type::Mouse, {}, button);
}

AbilityControl::AbilityControl(Type controlType, std::vector<sf::Keyboard::Key> controlKeys,
    sf::Mouse::Button button)
    : type(controlType), keys(std::move(controlKeys)), mouseButton(button)
{
}

bool AbilityControl::isPressed() const
{
    if (type == Type::Mouse)
        return sf::Mouse::isButtonPressed(mouseButton);

    for (const sf::Keyboard::Key key : keys)
    {
        if (!sf::Keyboard::isKeyPressed(key))
            return false;
    }

    return !keys.empty();
}

Ability::Ability(std::string abilitySlug, std::string abilityDisplayName,
    AbilityControl abilityControl, std::function<void()> abilityCallback)
    : slug(std::move(abilitySlug)), displayName(std::move(abilityDisplayName)),
      control(std::move(abilityControl)), callback(std::move(abilityCallback))
{
}

bool Ability::isTriggered()
{
    const bool pressed = control.isPressed();
    const bool triggered = pressed && !wasPressed;
    wasPressed = pressed;
    return triggered;
}

void Ability::activate() const
{
    if (callback)
        callback();
}

const std::string& Ability::getSlug() const
{
    return slug;
}

const std::string& Ability::getDisplayName() const
{
    return displayName;
}