#pragma once

#include <SDL3/SDL.h>
#include <vector>

class Player
{
public:
    Player();

    void Update(const bool* keyboardState, float deltaTime,
        const std::vector<SDL_FRect>& walls, float worldWidth, float worldHeight);
    void Reset();
    SDL_FRect GetRect() const;

    float x;
    float y;
    float width;
    float height;

private:
    float moveSpeed;
};
