#pragma once

#include <SDL3/SDL.h>

class Player
{
public:
    Player();

    void Update(const bool* keyboardState, float deltaTime);
    SDL_FRect GetRect() const;

    float x;
    float y;

    float width;
    float height;

private:
    float moveSpeed;
};