#pragma once

#include "GameState.hpp"
#include "GameSystems.hpp"
#include "../engine/Registry.hpp"

#include <SDL3/SDL.h>

class Game
{
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void Run();

private:
    void ProcessInput();
    void Update();
    void Render();
    void Reset();

    void CreateWall(const SDL_FRect& rect);
    void CreatePlayer();
    void CreateTarget();
    void CreatePickup();

    static constexpr int worldWidth = 1600;
    static constexpr int worldHeight = 1200;
    static constexpr World world{ static_cast<float>(worldWidth), static_cast<float>(worldHeight) };

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    bool fullscreen = false;

    Uint64 lastFrameTime = 0;
    float deltaTime = 0.0f;

    Registry registry;
    GameState state;
};
