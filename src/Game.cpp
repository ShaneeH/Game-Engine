#include "Game.hpp"

#include <iostream>

Game::Game()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Shane Engine",
        1600,
        1200,
        0
    );

    renderer = SDL_CreateRenderer(window, NULL);

    running = true;

    // Timing

    lastFrameTime = SDL_GetTicks();
    deltaTime = 0.0f;

    // Score

    score = 0;

    // Player

    rectX = 100;
    rectY = 200;

    rectWidth = 200;
    rectHeight = 150;

    moveSpeed = 300.0f;

    // Enemy

    enemyX = 700;
    enemyY = 300;

    enemyWidth = 250;
    enemyHeight = 200;

    enemyVelocityY = 200.0f;

    collided = false;

    std::cout << "Engine Started\n";
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::Run()
{
    while (running)
    {
        ProcessInput();
        Update();
        Render();

        SDL_Delay(16);
    }
}

void Game::ProcessInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
        }
    }

    const bool* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_W])
    {
        rectY -= moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_S])
    {
        rectY += moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_A])
    {
        rectX -= moveSpeed * deltaTime;
    }

    if (keyboardState[SDL_SCANCODE_D])
    {
        rectX += moveSpeed * deltaTime;
    }
}

void Game::Update()
{
    Uint64 currentTime = SDL_GetTicks();

    deltaTime = (currentTime - lastFrameTime) / 1000.0f;

    lastFrameTime = currentTime;

    // Enemy movement

    enemyY += enemyVelocityY * deltaTime;

    if (enemyY >= 900 || enemyY <= 0)
    {
        enemyVelocityY *= -1;
    }

    // Screen bounds

    if (rectX < 0)
    {
        rectX = 0;
    }

    if (rectX + rectWidth > 1600)
    {
        rectX = 1600 - rectWidth;
    }

    if (rectY < 0)
    {
        rectY = 0;
    }

    if (rectY + rectHeight > 1200)
    {
        rectY = 1200 - rectHeight;
    }

    // Collision

    if (rectX < enemyX + enemyWidth &&
        rectX + rectWidth > enemyX &&
        rectY < enemyY + enemyHeight &&
        rectY + rectHeight > enemyY)
    {
        collided = true;
    }
    else
    {
        collided = false;
    }

    // Score

    static bool wasColliding = false;

    if (collided && !wasColliding)
    {
        score++;
        std::cout << "Score: " << score << "\n";
    }

    wasColliding = collided;
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);

    SDL_RenderClear(renderer);

    SDL_FRect playerRect =
    {
        rectX,
        rectY,
        rectWidth,
        rectHeight
    };

    SDL_FRect enemyRect =
    {
        enemyX,
        enemyY,
        enemyWidth,
        enemyHeight
    };

    // Player

    if (collided)
    {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_RenderFillRect(renderer, &playerRect);

    // Enemy

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    SDL_RenderFillRect(renderer, &enemyRect);

    SDL_RenderPresent(renderer);
}