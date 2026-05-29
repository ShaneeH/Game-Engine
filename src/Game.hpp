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

    // Player

    float rectX;
    float rectY;

    float rectWidth;
    float rectHeight;

    float moveSpeed;
    float velocityX;

    // Enemy

    float enemyX;
    float enemyY;

    float enemyWidth;
    float enemyHeight;

    float enemyVelocityY;

    bool collided;

    // Timing

    Uint64 lastFrameTime;
    float deltaTime;

    // Score

    int score;
};