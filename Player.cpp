#include "Player.hpp"
#include "Collision.hpp"

#include <algorithm>
#include <cmath>

Player::Player()
{
    width = 64.0f;
    height = 64.0f;
    moveSpeed = 300.0f;
    Reset();
}

void Player::Reset()
{
    x = 100.0f;
    y = 200.0f;
}

void Player::Update(const bool* keyboardState, float deltaTime,
    const std::vector<SDL_FRect>& walls, float worldWidth, float worldHeight)
{
    float directionX = 0.0f;
    float directionY = 0.0f;

    if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) directionY -= 1.0f;
    if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) directionY += 1.0f;
    if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) directionX -= 1.0f;
    if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) directionX += 1.0f;

    float length = std::sqrt(directionX * directionX + directionY * directionY);
    if (length > 0.0f)
    {
        directionX /= length;
        directionY /= length;
    }

    float speed = moveSpeed;
    if (keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT])
    {
        speed *= 1.7f;
    }

    deltaTime = std::clamp(deltaTime, 0.0f, 0.05f);

    // Move each axis separately so the player can slide along walls.
    x += directionX * speed * deltaTime;
    for (const SDL_FRect& wall : walls)
    {
        if (IsColliding(GetRect(), wall))
        {
            if (directionX > 0.0f) x = wall.x - width;
            if (directionX < 0.0f) x = wall.x + wall.w;
        }
    }
    x = std::clamp(x, 0.0f, worldWidth - width);

    y += directionY * speed * deltaTime;
    for (const SDL_FRect& wall : walls)
    {
        if (IsColliding(GetRect(), wall))
        {
            if (directionY > 0.0f) y = wall.y - height;
            if (directionY < 0.0f) y = wall.y + wall.h;
        }
    }
    y = std::clamp(y, 0.0f, worldHeight - height);
}

SDL_FRect Player::GetRect() const
{
    return { x, y, width, height };
}
