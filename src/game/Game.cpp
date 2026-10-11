#include "Game.hpp"
#include "GameComponents.hpp"
#include "../engine/Components.hpp"
#include "../engine/Systems.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>

Game::Game()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }

    window = SDL_CreateWindow("Shane Engine", worldWidth, worldHeight, SDL_WINDOW_RESIZABLE);
    if (window)
    {
        renderer = SDL_CreateRenderer(window, nullptr);
    }

    if (!renderer)
    {
        std::string error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(error);
    }

    SDL_SetRenderLogicalPresentation(renderer, worldWidth, worldHeight,
        SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Reset();
    running = true;
    lastFrameTime = SDL_GetTicks();
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
        Uint64 currentTime = SDL_GetTicks();
        deltaTime = std::min((currentTime - lastFrameTime) / 1000.0f, 0.05f);
        lastFrameTime = currentTime;

        ProcessInput();
        if (!running) break;
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
        if (event.type == SDL_EVENT_QUIT) running = false;
        if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) state.paused = true;

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
        {
            if (event.key.key == SDLK_ESCAPE) running = false;
            if (event.key.key == SDLK_P && !state.gameOver) state.paused = !state.paused;
            if (event.key.key == SDLK_R) Reset();
            if (event.key.key == SDLK_F11)
            {
                if (SDL_SetWindowFullscreen(window, !fullscreen))
                {
                    fullscreen = !fullscreen;
                }
            }
        }
    }
}

void Game::Reset()
{
    state.score = 0;
    state.targetsCaught = 0;
    state.timeLeft = GameState::roundLength;
    state.paused = false;
    state.gameOver = false;

    registry.Clear();
    CreateWall({ 350.0f, 300.0f, 60.0f, 330.0f });
    CreateWall({ 700.0f, 750.0f, 350.0f, 60.0f });
    CreateWall({ 1150.0f, 250.0f, 60.0f, 350.0f });
    CreatePlayer();
    CreateTarget();
    for (int i = 0; i < 6; i++) CreatePickup();
}

void Game::CreateWall(const SDL_FRect& rect)
{
    Entity wall = registry.Create();
    registry.Add<Transform>(wall, rect.x, rect.y);
    registry.Add<Collider>(wall, rect.w, rect.h);
    registry.Add<Solid>(wall);
    registry.Add<Wall>(wall);
    registry.Add<Renderable>(wall, SDL_Color{ 85, 90, 110, 255 }, 0);
}

void Game::CreatePlayer()
{
    Entity player = registry.Create();
    registry.Add<Transform>(player, 100.0f, 200.0f);
    registry.Add<Velocity>(player);
    registry.Add<Collider>(player, 64.0f, 64.0f, CollisionResponse::Slide);
    registry.Add<Bounds>(player, SDL_FRect{ 0.0f, 0.0f, world.width, world.height });
    registry.Add<PlayerControl>(player);
    registry.Add<Renderable>(player, SDL_Color{ 235, 75, 85, 255 }, 3, SDL_Color{ 80, 230, 130, 255 });
}

void Game::CreateTarget()
{
    Target target;
    Entity entity = registry.Create();
    registry.Add<Transform>(entity, 700.0f, 300.0f);
    registry.Add<Velocity>(entity, 0.0f, target.baseSpeed);
    registry.Add<Collider>(entity, 80.0f, 80.0f, CollisionResponse::Bounce);
    // The top 100 pixels are kept clear for the HUD.
    registry.Add<Bounds>(entity, SDL_FRect{ 0.0f, 100.0f, world.width, world.height - 100.0f });
    registry.Add<Target>(entity, target);
    registry.Add<Renderable>(entity, SDL_Color{ 65, 135, 245, 255 }, 2);
}

void Game::CreatePickup()
{
    SDL_FRect rect = { 0.0f, 0.0f, 24.0f, 24.0f };
    if (!FindSpawn(registry, world, rect)) return;

    Entity pickup = registry.Create();
    registry.Add<Transform>(pickup, rect.x, rect.y);
    registry.Add<Collider>(pickup, rect.w, rect.h);
    registry.Add<Pickup>(pickup);
    registry.Add<Renderable>(pickup, SDL_Color{ 245, 200, 65, 255 }, 1);
}

void Game::Update()
{
    if (state.paused || state.gameOver) return;

    state.timeLeft = std::max(0.0f, state.timeLeft - deltaTime);
    if (state.timeLeft == 0.0f)
    {
        state.gameOver = true;
        return;
    }

    FlashSystem(registry, deltaTime);
    InputSystem(registry, SDL_GetKeyboardState(nullptr));
    MovementSystem(registry, deltaTime);
    PickupSystem(registry, state, world);
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 24, 24, 36, 255);
    for (int x = 0; x < worldWidth; x += 80)
    {
        SDL_RenderLine(renderer, static_cast<float>(x), 0.0f, static_cast<float>(x), worldHeight);
    }
    for (int y = 0; y < worldHeight; y += 80)
    {
        SDL_RenderLine(renderer, 0.0f, static_cast<float>(y), worldWidth, static_cast<float>(y));
    }

    RenderSystem(registry, renderer);
    HudSystem(renderer, state);
    SDL_RenderPresent(renderer);
}
