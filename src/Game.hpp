#pragma once

#include <SDL3/SDL.h>
#include "../Player.hpp"

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

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool running;

    Uint64 lastFrameTime;
    float deltaTime;

    int score;
    int highScore;

    Player player;

    float enemyX;
    float enemyY;

    float enemyWidth;
    float enemyHeight;

    float enemyVelocityY;

    bool collided;
};