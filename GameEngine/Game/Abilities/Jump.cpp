#include "Jump.h"

#include "../../Core/Player.h"

namespace
{
    constexpr float jumpVelocity = -500.0f;
}

void Jump::execute(Player& player)
{
    std::cout << "Jump executed.\n";
    if (player.isGrounded())
    {
        player.setVelocity(player.velocity.x, jumpVelocity);
        player.animation.start("assets/sprites/penitent_jump_anim.png", 4, 2, 0.12f);
    }
}