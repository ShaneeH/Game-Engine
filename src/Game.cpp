#include "Game.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

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

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    lastFrameTime = SDL_GetTicks();
    deltaTime = 0.0f;

    score = 0;
    highScore = 0;

    enemyX = 700.0f;
    enemyY = 300.0f;

    enemyWidth = 250.0f;
    enemyHeight = 200.0f;

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

    player.Update(keyboardState, deltaTime);
}

void Game::Update()
{
    Uint64 currentTime = SDL_GetTicks();

    deltaTime = (currentTime - lastFrameTime) / 1000.0f;

    lastFrameTime = currentTime;

    enemyY += enemyVelocityY * deltaTime;

    if (enemyY >= 900.0f || enemyY <= 0.0f)
    {
        enemyVelocityY *= -1.0f;
    }

    if (player.x < 0.0f)
    {
        player.x = 0.0f;
    }

    if (player.x + player.width > 1600.0f)
    {
        player.x = 1600.0f - player.width;
    }

    if (player.y < 0.0f)
    {
        player.y = 0.0f;
    }

    if (player.y + player.height > 1200.0f)
    {
        player.y = 1200.0f - player.height;
    }

    if (player.x < enemyX + enemyWidth &&
        player.x + player.width > enemyX &&
        player.y < enemyY + enemyHeight &&
        player.y + player.height > enemyY)
    {
        collided = true;
    }
    else
    {
        collided = false;
    }

    static bool wasColliding = false;

    if (collided && !wasColliding)
    {
        score++;

        if (score > highScore)
        {
            highScore = score;
        }

        enemyX = static_cast<float>(std::rand() % 1300);
        enemyY = static_cast<float>(std::rand() % 900);

        std::cout
            << "Score: "
            << score
            << " | High Score: "
            << highScore
            << "\n";
    }

    wasColliding = collided;

    std::string title =
        "Shane Engine - Score: " +
        std::to_string(score) +
        " High Score: " +
        std::to_string(highScore);

    SDL_SetWindowTitle(window, title.c_str());
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);

    SDL_RenderClear(renderer);

    SDL_FRect playerRect = player.GetRect();

    SDL_FRect enemyRect =
    {
        enemyX,
        enemyY,
        enemyWidth,
        enemyHeight
    };

    if (collided)
    {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    SDL_RenderFillRect(renderer, &enemyRect);

    SDL_RenderPresent(renderer);
}