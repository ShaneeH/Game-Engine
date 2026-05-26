#pragma once

#include <SDL3/SDL.h>

class Game
{
public:
    Game();
    ~Game();

    void Run();

private:
    void ProcessInput();
    void Update();
    void Render();

    bool running;

    SDL_Window* window;
    SDL_Renderer* renderer;

    float rectX;
    float rectY;

    float velocityX;
};