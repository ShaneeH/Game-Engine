#include "Game.hpp"

#include <iostream>

Game::Game()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Shane Engine",
        800,
        600,
        0
    );

    renderer = SDL_CreateRenderer(window, NULL);

    running = true;

    rectX = 100;
    rectY = 200;

    velocityX = 0.2f;

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
}

void Game::Update()
{
    rectX += velocityX;

    if (rectX >= 600 || rectX <= 0)
    {
        velocityX *= -1;
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);

    SDL_RenderClear(renderer);

    SDL_FRect rect = { rectX, rectY, 200, 150 };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}