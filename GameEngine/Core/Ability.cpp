#include "Ability.h"

#include <utility>

AbilityControl AbilityControl::keyboard(std::vector<sf::Keyboard::Key> keys)
{
    return AbilityControl(Type::Keyboard, std::move(keys), sf::Mouse::Button::Left, true);
}

AbilityControl AbilityControl::keyboardAny(std::vector<sf::Keyboard::Key> keys)
{
    return AbilityControl(Type::Keyboard, std::move(keys), sf::Mouse::Button::Left, false);
}

AbilityControl AbilityControl::always()
{
    return AbilityControl(Type::Always, {}, sf::Mouse::Button::Left, true);
}

AbilityControl AbilityControl::mouse(sf::Mouse::Button button)
{
    return AbilityControl(Type::Mouse, {}, button, true);
}

AbilityControl::AbilityControl(Type controlType, std::vector<sf::Keyboard::Key> controlKeys,
        sf::Mouse::Button button, bool shouldRequireAllKeys)
    : type(controlType), keys(std::move(controlKeys)), mouseButton(button),
      requireAllKeys(shouldRequireAllKeys)
{
}

bool AbilityControl::isPressed() const
{
    if (type == Type::Always)
        return true;

    if (type == Type::Mouse)
        return sf::Mouse::isButtonPressed(mouseButton);

    if (!requireAllKeys)
    {
        for (const sf::Keyboard::Key key : keys)
        {
            if (sf::Keyboard::isKeyPressed(key))
                return true;
        }

        return false;
    }

    for (const sf::Keyboard::Key key : keys)
    {
        if (!sf::Keyboard::isKeyPressed(key))
            return false;
    }

    return !keys.empty();
}

Ability::Ability(std::string abilitySlug, std::string abilityDisplayName,
        AbilityControl abilityControl, std::function<void()> abilityCallback, bool isContinuous)
    : slug(std::move(abilitySlug)), displayName(std::move(abilityDisplayName)),
            control(std::move(abilityControl)), callback(std::move(abilityCallback)),
            continuous(isContinuous)
{
}

bool Ability::isTriggered()
{
    const bool pressed = control.isPressed();
    const bool triggered = pressed && !wasPressed;
    wasPressed = pressed;
    return triggered;
}

bool Ability::isActive() const
{
    return control.isPressed();
}

bool Ability::isContinuous() const
{
    return continuous;
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