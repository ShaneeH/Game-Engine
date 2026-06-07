#include "Player.hpp"

Player::Player()
{
    x = 100.0f;
    y = 200.0f;

    width = 200.0f;
    height = 150.0f;

    moveSpeed = 300.0f;
}

void Player::Update(const bool* keyboardState, float deltaTime)
{
    if (keyboardState[SDL_SCANCODE_W])
    {
        y -= moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_S])
    {
        y += moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_A])
    {
        x -= moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_D])
    {
        x += moveSpeed * deltaTime;
    }
}

SDL_FRect Player::GetRect() const
{
    SDL_FRect rect =
    {
        x,
        y,
        width,
        height
    };

    return rect;
}