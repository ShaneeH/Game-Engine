#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "../Player.hpp"

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
    void AddScore(int points);
    bool FindSpawn(SDL_FRect& rect);
    bool IsSpawnFree(const SDL_FRect& rect) const;

    static constexpr int worldWidth = 1600;
    static constexpr int worldHeight = 1200;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    bool paused = false;
    bool gameOver = false;
    bool fullscreen = false;

    Uint64 lastFrameTime = 0;
    float deltaTime = 0.0f;
    float timeLeft = 60.0f;
    float collectFlash = 0.0f;

    int score = 0;
    int highScore = 0;
    int targetsCaught = 0;

    Player player;
    SDL_FRect enemyRect = { 700.0f, 300.0f, 80.0f, 80.0f };
    float enemyVelocityY = 200.0f;
    std::vector<SDL_FRect> walls;
    std::vector<SDL_FRect> coins;
};
