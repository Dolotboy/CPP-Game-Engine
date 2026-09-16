#include "Jump.h"

#include "Player.h"

namespace
{
    constexpr float jumpVelocity = -500.0f;
}

void Jump::execute(Player& player)
{
    std::cout << "Jump executed.\n";
    if (player.isGrounded())
        player.setVelocity(player.velocity.x, jumpVelocity);
}